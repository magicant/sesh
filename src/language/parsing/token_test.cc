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
#include "common/type_tag.hh"
#include "common/visitor.hh"
#include "common/visitor_test_helper.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/token.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"

namespace {

using sesh::common::make_checking_visitor;
using sesh::common::type_tag;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::parse_token;
using sesh::language::parsing::token;
using sesh::language::parsing::token_type_set;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;

TEST_CASE("Token parser fails if type set is empty") {
    check_parser_failure(
            std::bind(parse_token, token_type_set{}, std::placeholders::_1),
            {});
}

const auto parse_word_token = std::bind(
        parse_token, token_type_set{type_tag<word>()}, std::placeholders::_1);

TEST_CASE("Token parser fails if word is empty") {
    check_parser_failure(parse_word_token, L(";"));
}

void expect_raw_string_word(const token &t) {
    REQUIRE(t.tag() == t.tag<word>());
    const auto &w = t.value<word>();
    REQUIRE(w.components.size() == 1);
    const auto &wc = w.components[0];
    REQUIRE(wc != nullptr);
    auto checker = [](const raw_string &rs) {
        CHECK(rs.value == L("aa"));
    };
    visit(*wc, make_checking_visitor<raw_string>(checker));
}

TEST_CASE("Token parser parses word") {
    check_parser_success_result(
            parse_word_token, L("aa;"), expect_raw_string_word);
}

TEST_CASE("Token parser skips parsed word") {
    check_parser_success_rest(parse_word_token, L("aa"), L(";"));
}

TEST_CASE("Token parser skips leading line continuations") {
    check_parser_success_result(
            parse_word_token, L("\\\n\\\naa;"), expect_raw_string_word);
}

TEST_CASE("Token parser is context-free with raw string word") {
    check_parser_success_context_free(parse_word_token, L("aa;"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
