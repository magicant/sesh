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
#include <memory>
#include <typeinfo>
#include "catch.hpp"
#include "common/constant_function.hh"
#include "common/visitor.hh"
#include "common/visitor_test_helper.hh"
#include "common/xchar.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/word_component.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word_component.hh"

namespace {

using sesh::common::constant;
using sesh::common::make_checking_visitor;
using sesh::common::xchar;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::context;
using sesh::language::parsing::parse_word_component;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word_component;

constexpr bool is_a(xchar x, const context &) noexcept { return x == L('a'); }

const auto parse_a_word_component =
        std::bind(parse_word_component, is_a, std::placeholders::_1);

TEST_CASE("Empty string is not word component") {
    using std::placeholders::_1;
    check_parser_failure(
            std::bind(parse_word_component, constant(true), _1), {});
}

TEST_CASE("Raw string is word component") {
    check_parser_success_result(
            parse_a_word_component,
            L("abc"),
            [](const std::shared_ptr<const word_component> &wc) {
                REQUIRE(wc != nullptr);
                auto checker = [](const raw_string &rs) {
                    CHECK(rs.value == L("a"));
                };
                visit(*wc, make_checking_visitor<raw_string>(checker));
            });
}

TEST_CASE("Word component parser leaves unparsed part") {
    check_parser_success_rest(parse_a_word_component, L("a"), L("bc"));
}

TEST_CASE("Word component parser is context-free with raw string") {
    check_parser_success_context_free(parse_a_word_component, L("abc"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
