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

#include <tuple>
#include "catch.hpp"
#include "common/xchar.hh"
#include "language/parsing/line_continuation.hh"
#include "language/parsing/parser_test_helper.hh"

namespace {

using sesh::common::xchar;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_no_excess;
using sesh::language::parsing::check_parser_no_reports;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::skip_line_continuation;
using sesh::language::parsing::skip_line_continuations;

TEST_CASE("EOF is not line continuation") {
    check_parser_failure(skip_line_continuation, {});
}

TEST_CASE("X is not line continuation") {
    check_parser_failure(skip_line_continuation, L("X"));
}

TEST_CASE("Backslash followed by EOF is not line continuation") {
    check_parser_failure(skip_line_continuation, L("\\"));
}

TEST_CASE("Backslash followed by X is not line continuation") {
    check_parser_failure(skip_line_continuation, L("\\X"));
}

TEST_CASE("skip_line_continuation reports nothing on failure") {
    check_parser_no_reports(skip_line_continuation, {});
}

TEST_CASE("Backslash followed by newline is line continuation") {
    check_parser_success_result(
            skip_line_continuation,
            L("\\\n"),
            [](const std::tuple<xchar, xchar> &t) {
                CHECK(std::get<0>(t) == L('\\'));
                CHECK(std::get<1>(t) == L('\n'));
            });
}

TEST_CASE("skip_line_continuation skips line continuation only") {
    check_parser_success_rest(skip_line_continuation, L("\\\n"));
}

TEST_CASE("skip_line_continuation is context-free") {
    check_parser_success_context_free(skip_line_continuation, L("\\\n"));
}

TEST_CASE("skip_line_continuation don't read beyond line continuation") {
    check_parser_no_excess(skip_line_continuation, L("\\\n"));
}

TEST_CASE("skip_line_continuations succeeds with no line continuation") {
    check_parser_success_rest(skip_line_continuations, {});
}

TEST_CASE("skip_line_continuations succeeds with two line continuations") {
    check_parser_success_rest(skip_line_continuations, L("\\\n\\\n"), L("X"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
