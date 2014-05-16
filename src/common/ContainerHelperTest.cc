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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <map>
#include <string>
#include <vector>
#include "common/ContainerHelper.hh"
#include "common/String.hh" // for test of overloading

namespace {

using sesh::common::contains;
using sesh::common::find;

TEST_CASE("Container helper: find first 2 in vector {1, 2, 5, 2}") {
    std::vector<int> v{1, 2, 5, 2};
    CHECK(find(v, 2) == v.begin() + 1);
}

TEST_CASE("Container helper: doesn't find 3 in vector {1, 2, 5, 2}") {
    std::vector<int> v{1, 2, 5, 2};
    CHECK(find(v, 3) == v.end());
}

TEST_CASE("Container helper: vector {1, 2, 5} contains 2") {
    CHECK(contains(std::vector<int>{1, 2, 5}, 2));
}

TEST_CASE("Container helper: vector {1, 2, 5} doesn't contain 3") {
    CHECK_FALSE(contains(std::vector<int>{1, 2, 5}, 3));
}

TEST_CASE("Container helper: find 1 in map {{1, true}, {3, false}}") {
    std::map<int, bool> m{{1, true}, {3, false}};
    CHECK(find(m, 1) == m.begin());
}

TEST_CASE("Container helper: doesn't find 2 in map {{1, true}, {3, false}}") {
    std::map<int, bool> m{{1, true}, {3, false}};
    CHECK(find(m, 2) == m.end());
}

TEST_CASE("Container helper: map {{1, true}, {3, false}} contains 1") {
    std::map<int, bool> m{{1, true}, {3, false}};
    CHECK(contains(m, 1));
}

TEST_CASE("Container helper: map {{1, true}, {3, false}} doesn't contain 2") {
    std::map<int, bool> m{{1, true}, {3, false}};
    CHECK_FALSE(contains(m, 2));
}

TEST_CASE("Container helper: array {1, 4, 6} contains 6") {
    int a[] = {1, 4, 6};
    CHECK(contains(&a[0], &a[3], 6));
}

TEST_CASE("Container helper: array {1, 4} doesn't contain 6") {
    int a[] = {1, 4, 6};
    CHECK_FALSE(contains(&a[0], &a[2], 6));
}

TEST_CASE("String \"hello\" contains 'l'") {
    std::string s = "hello";
    CHECK(contains(s, 'l'));
}

TEST_CASE("String \"hello\" doesn't 'x'") {
    std::string s = "hello";
    CHECK_FALSE(contains(s, 'x'));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
