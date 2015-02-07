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
#include "async/future_test_helper.hh"
#include "catch.hpp"
#include "common/either.hh"
#include "common/xchar.hh"
#include "language/executing/expansion.hh"
#include "language/executing/expansion_result.hh"
#include "language/executing/word.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component_test_helper.hh"

namespace {

using sesh::common::trial;
using sesh::language::executing::expand;
using sesh::language::executing::expansion;
using sesh::language::executing::expansion_result;
using sesh::language::syntax::make_word_component_stub;
using sesh::language::syntax::word;

TEST_CASE("Empty word expands to empty result") {
    auto w = std::make_shared<word>();
    expect_result(
            expand(nullptr, false, w),
            [](expansion_result &&result) {
        CHECK(result.reports.empty());
        REQUIRE(result.words);
        CHECK(result.words->empty());
    });
}

TEST_CASE("Expanding single raw string component word") {
    auto w = std::make_shared<word>();
    w->components.push_back(make_word_component_stub(L("foo")));
    expect_result(
            expand(nullptr, false, w),
            [](expansion_result &&result) {
        CHECK(result.reports.empty());
        REQUIRE(result.words);
        CHECK(result.words->size() == 1);

        const expansion &word = result.words->front();
        CHECK(word.characters.size() == 3);
        CHECK(word.characters.at(0).character == L('f'));
    });
}

TEST_CASE("Expanding double raw string component word") {
    auto w = std::make_shared<word>();
    w->components.push_back(make_word_component_stub(L("foo")));
    w->components.push_back(make_word_component_stub(L("bar")));
    expect_result(
            expand(nullptr, false, w),
            [](expansion_result &&result) {
        CHECK(result.reports.empty());
        REQUIRE(result.words);
        CHECK(result.words->size() == 1);

        const expansion &word = result.words->front();
        CHECK(word.characters.size() == 6);
        CHECK(word.characters.at(0).character == L('f'));
        CHECK(word.characters.at(3).character == L('b'));
    });
}

TEST_CASE("Expanding triple raw string component word") {
    auto w = std::make_shared<word>();
    w->components.push_back(make_word_component_stub(L("A")));
    w->components.push_back(make_word_component_stub(L("B")));
    w->components.push_back(make_word_component_stub(L("C")));
    expect_result(
            expand(nullptr, false, w),
            [](expansion_result &&result) {
        CHECK(result.reports.empty());
        REQUIRE(result.words);
        CHECK(result.words->size() == 1);

        const expansion &word = result.words->front();
        CHECK(word.characters.size() == 3);
        CHECK(word.characters.at(0).character == L('A'));
        CHECK(word.characters.at(1).character == L('B'));
        CHECK(word.characters.at(2).character == L('C'));
    });
}

// TODO: Expansion in quotation
// TODO: Expansion of $@

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
