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

#include <functional>
#include "catch.hpp"
#include "common/xchar.hh"
#include "language/parsing/line_continued_char.hh"
#include "language/parsing/parser_test_helper.hh"

namespace {

using sesh::common::xchar;
using sesh::language::parsing::accept_char_after_line_continuations;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::parse_char_after_line_continuations;

TEST_CASE("parse_char_after_line_continuations succeeds for matching char") {
    using namespace std::placeholders;
    check_parser_success_result(
            std::bind(parse_char_after_line_continuations, L('a'), _1),
            L("a"),
            [](xchar c) { CHECK(c == L('a')); });
}

TEST_CASE(
        "parse_char_after_line_continuations succeeds for matching char "
        "after skipping line continuations") {
    using namespace std::placeholders;
    check_parser_success_result(
            std::bind(parse_char_after_line_continuations, L('a'), _1),
            L("\\\n\\\n\\\na"),
            [](xchar c) { CHECK(c == L('a')); });
}

TEST_CASE("parse_char_after_line_continuations fails for unmatched char") {
    using namespace std::placeholders;
    check_parser_failure(
            std::bind(parse_char_after_line_continuations, L('a'), _1),
            L("\\\n\\\n\\\nb"));
}

TEST_CASE("accept_char_after_line_continuations succeeds for any char") {
    using namespace std::placeholders;
    check_parser_success_result(
            accept_char_after_line_continuations,
            L("!"),
            [](xchar c) { CHECK(c == L('!')); });
}

TEST_CASE("accept_char_after_line_continuations fails at end of input") {
    check_parser_failure(accept_char_after_line_continuations, {});
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
