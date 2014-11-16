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
#include "common/xstring.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/raw_string.hh"
#include "language/syntax/raw_string.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::parse_raw_string;
using sesh::language::syntax::raw_string;

constexpr bool is_a(xchar c) noexcept { return c == L('a'); }

TEST_CASE("Raw string parser rejects empty result") {
    using namespace std::placeholders;
    check_parser_failure(std::bind(parse_raw_string, is_a, _1), {});
}

TEST_CASE("Raw string parser with 1-character result") {
    using namespace std::placeholders;
    check_parser_success_result(
            std::bind(parse_raw_string, is_a, _1),
            L("ax"),
            [](const raw_string &s) { CHECK(s.value == L("a")); });
}

TEST_CASE("Raw string parser with 3-character result") {
    using namespace std::placeholders;
    check_parser_success_result(
            std::bind(parse_raw_string, is_a, _1),
            L("aaax"),
            [](const raw_string &s) { CHECK(s.value == L("aaa")); });
}

TEST_CASE("Raw string parser is context-free") {
    using namespace std::placeholders;
    check_parser_success_context_free(
            std::bind(parse_raw_string, is_a, _1), L("aaax"));
}

TEST_CASE("Raw string parser leaves unparsed part of source") {
    using namespace std::placeholders;
    check_parser_success_rest(
            std::bind(parse_raw_string, is_a, _1), L("aaa"), L("x"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
