/* Copyright (C) 2014 WATANABE Yuki
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

#include <algorithm>
#include <memory>
#include <vector>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/visitor.hh"
#include "common/visitor_test_helper.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/simple_command.hh"
#include "language/source/fragment.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/word_test_helper.hh"
#include "ui/message/category.hh"
#include "ui/message/format.hh"
#include "ui/message/report.hh"
#include "ui/message/report_test_helper.hh"

namespace {

using sesh::common::copy;
using sesh::common::xstring;
using sesh::language::parsing::parse_simple_command;
using sesh::language::parsing::simple_command_parse;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::syntax::simple_command;
using sesh::ui::message::category;
using sesh::ui::message::format;
using sesh::ui::message::report;

TEST_CASE("Simple command parser fails for empty command") {
    check_parser_failure(parse_simple_command, L(";"));
}

TEST_CASE("Simple command parser reports empty command as error") {
    fragment_position fp(std::make_shared<fragment>(L(";")));
    check_parser_reports_with_fragment(
            parse_simple_command,
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

void expect_raw_string_word_command_parse(
        const simple_command_parse &actual_parse,
        const xstring &expected_string) {
    REQUIRE(actual_parse.tag() == actual_parse.tag<simple_command>());
    const auto &sc = actual_parse.value<simple_command>();
    REQUIRE(sc.words.size() == 1);
    const auto &w = sc.words[0];
    expect_raw_string_word(w, expected_string);
}

TEST_CASE("Simple command parser succeeds with single word command") {
    check_parser_success_result(
            parse_simple_command,
            L("command"),
            [](const simple_command_parse &p) {
                expect_raw_string_word_command_parse(p, L("command"));
            });
}

TEST_CASE("Simple command parser succeeds with double word command") {
    check_parser_success_result(
            parse_simple_command,
            L("command \\\n\targument;"),
            [](const simple_command_parse &p) {
                REQUIRE(p.tag() == p.tag<simple_command>());
                const auto &sc = p.value<simple_command>();
                REQUIRE(sc.words.size() == 2);
                expect_raw_string_word(sc.words[0], L("command"));
                expect_raw_string_word(sc.words[1], L("argument"));
            });
}

TEST_CASE("Simple command parser skips parsed tokens") {
    check_parser_success_rest(
            parse_simple_command,
            L("command \\\n\targument"),
            L(";"));
}

TEST_CASE("Simple command parser skips trailing blanks") {
    check_parser_success_rest(
            parse_simple_command,
            L("command \t\\\n#\\"),
            L("\n"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
