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
#include "language/parsing/char.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/parser_test_helper.hh"

namespace {

using sesh::common::xchar;
using sesh::language::parsing::accept_char;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_no_reports;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::context;
using sesh::language::parsing::parse_char;

TEST_CASE("parse_char succeeds for matching char") {
    using namespace std::placeholders;
    check_parser_success_result(
            std::bind(parse_char, L('a'), _1),
            L("a"),
            [](const xchar &c) { CHECK(c == L('a')); });
}

TEST_CASE("parse_char consumes the matching char") {
    using namespace std::placeholders;
    check_parser_success_rest(
            std::bind(parse_char, L('a'), _1), L("a"));
}

TEST_CASE("parse_char is context-free") {
    using namespace std::placeholders;
    check_parser_success_context_free(
            std::bind(parse_char, L('a'), _1), L("a"));
}

TEST_CASE("parse_char reads only one char on success") {
    using namespace std::placeholders;
    check_parser_no_excess(std::bind(parse_char, L('a'), _1), L("a"));
}

TEST_CASE("parse_char reports nothing on success") {
    using namespace std::placeholders;
    check_parser_no_reports(std::bind(parse_char, L('a'), _1), L("a"));
}

TEST_CASE("parse_char fails for unmatched char") {
    using namespace std::placeholders;
    check_parser_failure(std::bind(parse_char, L('a'), _1), L("."));
}

TEST_CASE("parse_char reads only one char on failure") {
    using namespace std::placeholders;
    check_parser_no_excess(std::bind(parse_char, L('a'), _1), L("."));
}

TEST_CASE("parse_char reports nothing on failure") {
    using namespace std::placeholders;
    check_parser_no_reports(std::bind(parse_char, L('a'), _1), L("."));
}

void check_accept_char_success(xchar c) {
    using namespace std::placeholders;
    check_parser_success_result(
            accept_char, {c}, [c](const xchar &c2) { CHECK(c2 == c); });
}

TEST_CASE("accept_char succeeds for any char: digit") {
    check_accept_char_success(L('7'));
}

TEST_CASE("accept_char succeeds for any char: null") {
    check_accept_char_success(L('\0'));
}

TEST_CASE("accept_char fails at end of input") {
    check_parser_failure(accept_char, {});
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
