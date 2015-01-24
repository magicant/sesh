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

#include <memory>
#include <vector>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/xchar.hh"
#include "language/parsing/command.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/source/fragment.hh"
#include "language/syntax/command_test_helper.hh"
#include "language/syntax/simple_command.hh"
#include "ui/message/category.hh"

namespace {

using sesh::common::copy;
using sesh::language::parsing::command_parse;
using sesh::language::parsing::command_pointer;
using sesh::language::parsing::parse_command;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::syntax::simple_command;
using sesh::ui::message::category;

TEST_CASE("Command parser fails for empty input") {
    check_parser_failure(parse_command, L(";"));
}

TEST_CASE("Command parser reports empty command as error") {
    check_parser_single_report(
            category::error, L("empty command"), parse_command, {}, L(";"));
}

TEST_CASE("Command parser parses simple command") {
    check_parser_success_result(
            parse_command,
            L("command  argument  ;"),
            [](const command_parse &p) {
                REQUIRE(p.tag() == p.tag<command_pointer>());
                const auto &c = p.value<command_pointer>();
                REQUIRE(c);
                expect_raw_string_command(*c, {L("command"), L("argument")});
            });
}

TEST_CASE("Command parser skips parsed simple command") {
    check_parser_success_rest(
            parse_command,
            L("command  argument  "),
            L(";"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
