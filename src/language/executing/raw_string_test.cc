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

#include "catch.hpp"
#include "common/either.hh"
#include "common/xchar.hh"
#include "language/executing/expansion_result.hh"
#include "language/executing/raw_string.hh"
#include "language/executing/result_test_helper.hh"
#include "language/syntax/raw_string.hh"

namespace {

using sesh::common::trial;
using sesh::language::executing::expand;
using sesh::language::executing::expansion_result;
using sesh::language::syntax::raw_string;

template<typename T>
std::shared_ptr<T> dummy_shared_ptr(T *t) noexcept {
    return std::shared_ptr<T>(std::shared_ptr<T>(), t);
}

void test_empty_raw_string(bool is_quoted) {
    raw_string s;
    bool called = false;
    expand(nullptr, is_quoted, dummy_shared_ptr(&s)).then(
            [&called](trial<expansion_result> &&result) {
        REQUIRE(result);
        check_null(*result);
        REQUIRE(result->words);
        REQUIRE(result->words->size() == 1);
        CHECK(result->words->front().characters.empty());
        called = true;
    });
    CHECK(called);
}

TEST_CASE("Expansion of empty unquoted raw string") {
    test_empty_raw_string(false);
}

TEST_CASE("Expansion of empty quoted raw string") {
    test_empty_raw_string(true);
}

void test_nonempty_raw_string(bool is_quoted) {
    raw_string s;
    s.value = L("ABC");
    bool called = false;
    expand(nullptr, is_quoted, dummy_shared_ptr(&s)).then(
            [is_quoted, &called](trial<expansion_result> &&result) {
        REQUIRE(result);
        check_null(*result);
        REQUIRE(result->words);
        REQUIRE(result->words->size() == 1);

        const auto &word = result->words->front();
        CHECK(word.characters.size() == 3);

        CHECK(word.characters.at(0).character == L('A'));
        CHECK(word.characters.at(0).is_literal);
        CHECK(word.characters.at(0).is_quoted == is_quoted);

        CHECK(word.characters.at(1).character == L('B'));
        CHECK(word.characters.at(1).is_literal);
        CHECK(word.characters.at(1).is_quoted == is_quoted);

        CHECK(word.characters.at(2).character == L('C'));
        CHECK(word.characters.at(2).is_literal);
        CHECK(word.characters.at(2).is_quoted == is_quoted);

        called = true;
    });
    CHECK(called);
}

TEST_CASE("Expansion of non-empty unquoted raw string") {
    test_nonempty_raw_string(false);
}

TEST_CASE("Expansion of non-empty quoted raw string") {
    test_nonempty_raw_string(true);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
