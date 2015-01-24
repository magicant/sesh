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
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/sequence.hh"
#include "language/source/fragment.hh"
#include "language/syntax/sequence.hh"
#include "language/syntax/sequence_test_helper.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/word_test_helper.hh"
#include "ui/message/category.hh"

namespace {

using sesh::common::copy;
using sesh::language::parsing::parse_sequence;
using sesh::language::parsing::sequence_parse;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::syntax::sequence;
using sesh::language::syntax::simple_command;
using sesh::ui::message::category;

TEST_CASE("Sequence parser fails for empty input") {
    check_parser_failure(parse_sequence, L(";"));
}

TEST_CASE("Sequence parser reports empty command as error") {
    check_parser_single_report(
            category::error, L("empty command"), parse_sequence, {}, L(";"));
}

TEST_CASE("Sequence parser parses simple command") {
    check_parser_success_result(
            parse_sequence,
            L("command  argument  ;"),
            [](const sequence_parse &sp) {
                REQUIRE(sp.tag() == sp.tag<sequence>());
                const auto &s = sp.value<sequence>();
                expect_raw_string_sequence(s, {L("command"), L("argument")});
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
