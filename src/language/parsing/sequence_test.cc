/* Copyright (C) 2015 WATANABE Yuki
 *
 * This file is part of Sesh.
 *
 * Sesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Sesh is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Sesh.  If not, see <http://www.gnu.org/licenses/>.  */

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/visitor_test_helper.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/sequence.hh"
#include "language/source/fragment.hh"
#include "language/syntax/sequence.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/word_test_helper.hh"
#include "ui/message/category.hh"
#include "ui/message/format.hh"
#include "ui/message/report_test_helper.hh"

namespace {

using sesh::common::copy;
using sesh::common::make_checking_visitor;
using sesh::language::parsing::parse_sequence;
using sesh::language::parsing::sequence_parse;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::syntax::sequence;
using sesh::language::syntax::simple_command;
using sesh::ui::message::category;
using sesh::ui::message::format;
using sesh::ui::message::report;

TEST_CASE("Sequence parser fails for empty input") {
    check_parser_failure(parse_sequence, L(";"));
}

TEST_CASE("Sequence parser reports empty command as error") {
    fragment_position fp(std::make_shared<fragment>(L(";")));
    check_parser_reports_with_fragment(
            parse_sequence,
            fp,
            [fp](const std::vector<report> &rs) {
                REQUIRE(rs.size() == 1);
                check_equal(
                        rs[0],
                        report(
                            category::error,
                            format<>(L("empty command")),
                            copy(fp)));
            });
}

TEST_CASE("Sequence parser parses simple command") {
    check_parser_success_result(
            parse_sequence,
            L("command  argument  ;"),
            [](const sequence_parse &sp) {
                REQUIRE(sp.tag() == sp.tag<sequence>());
                const auto &s = sp.value<sequence>();
                REQUIRE(s.and_or_lists.size() == 1);
                const auto &a = s.and_or_lists[0];
                REQUIRE(a.first.commands.size() == 1);
                const auto &c = *a.first.commands[0];
                const auto check = [](const simple_command &sc) {
                    REQUIRE(sc.words.size() == 2);
                    expect_raw_string_word(sc.words[0], L("command"));
                    expect_raw_string_word(sc.words[1], L("argument"));
                };
                visit(c, make_checking_visitor<simple_command>(check));
            });
}

TEST_CASE("Sequence parser skips parsed simple command") {
    check_parser_success_rest(
            parse_sequence,
            L("command  argument  "),
            L(";"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
