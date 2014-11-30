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
#include "common/visitor_test_helper.hh"
#include "common/xchar.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/word.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"

namespace {

using sesh::common::make_checking_visitor;
using sesh::common::xchar;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::context;
using sesh::language::parsing::parse_word;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;

constexpr bool is_a(xchar x, const context &) noexcept { return x == L('a'); }

const auto parse_a_word = std::bind(parse_word, is_a, std::placeholders::_1);

TEST_CASE("Empty string yields empty word") {
    check_parser_success_result(
            parse_a_word,
            {},
            [](const word &w) { CHECK(w.components.empty()); });
}

TEST_CASE("Non-word-char yields empty word") {
    check_parser_success_result(
            parse_a_word,
            L(";"),
            [](const word &w) { CHECK(w.components.empty()); });
}

TEST_CASE("Raw string is word") {
    auto checker = [](const word &w) {
        REQUIRE(w.components.size() == 1);
        const auto &wc = w.components[0];
        REQUIRE(wc != nullptr);
        auto checker = [](const raw_string &rs) {
            CHECK(rs.value == L("aa"));
        };
        visit(*wc, make_checking_visitor<raw_string>(checker));
    };
    check_parser_success_result(parse_a_word, L("\\\naa\\\n;"), checker);
}

TEST_CASE("Word parser leaves unparsed part") {
    check_parser_success_rest(parse_a_word, L("a"), L("bc"));
}

TEST_CASE("Word parser is context-free with raw string") {
    check_parser_success_context_free(parse_a_word, L("abc"));
}

// TODO test words with more than one component

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
