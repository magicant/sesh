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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "catch.hpp"
#include "common/container_helper.hh"
#include "common/xstring.hh" // for test of overloading

namespace {

using sesh::common::contains;
using sesh::common::find;
using sesh::common::make_vector_of;

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

TEST_CASE("Container helper: copy") {
    const std::vector<int> from = {1, 2, 3};
    std::vector<int> to;
    sesh::common::copy(from, to);
    CHECK(from == to);
}

TEST_CASE("Container helper: copy_if") {
    const std::vector<int> from = {1, 2, 3, 4, 5};
    std::vector<int> to;
    sesh::common::copy_if(from, to, [](int i) { return i % 2 == 0; });
    CHECK(to == (std::vector<int>{2, 4}));
}

TEST_CASE("Container helper: move") {
    struct move_only_int {
        int value;
        move_only_int(int i) noexcept : value(i) { }
        move_only_int(move_only_int &&) = default;
    };
    std::vector<move_only_int> from, to;
    from.emplace_back(3);
    sesh::common::move(from, to);
    REQUIRE(to.size() == 1);
    CHECK(to[0].value == 3);
}

TEST_CASE("Container helper: transform") {
    std::vector<int> from = {1, 2, 5};
    std::vector<double> to;
    sesh::common::transform(from, to, [](int v) { return v * 100.0; });
    REQUIRE(to.size() == 3);
    CHECK(to[0] == 100.0);
    CHECK(to[1] == 200.0);
    CHECK(to[2] == 500.0);
}

TEST_CASE("Container helper: move_transform") {
    struct transformer {
        constexpr double operator()(const int &) const { return 0.0; }
        constexpr double operator()(int &&v) const { return v * 100.0; }
    };
    std::vector<int> from = {1, 2, 5};
    std::vector<double> to;
    sesh::common::move_transform(from, to, transformer());
    REQUIRE(to.size() == 3);
    CHECK(to[0] == 100.0);
    CHECK(to[1] == 200.0);
    CHECK(to[2] == 500.0);
}

TEST_CASE("Container helper: create vector of {}") {
    std::vector<int> v = make_vector_of<int>();
    CHECK(v.empty());
}

TEST_CASE("Container helper: create vector of {1}") {
    std::vector<int> v = make_vector_of<int>(1);
    CHECK(v.size() == 1);
    CHECK(v.at(0) == 1);
}

TEST_CASE("Container helper: create vector of {2, 1}") {
    std::vector<int> v = make_vector_of<int>(2, 1);
    CHECK(v.size() == 2);
    CHECK(v.at(0) == 2);
    CHECK(v.at(1) == 1);
}

TEST_CASE("Container helper: create vector of {4, 2, 1, 3}") {
    std::vector<int> v = make_vector_of<int>(4, 2, 1, 3);
    CHECK(v.size() == 4);
    CHECK(v.at(0) == 4);
    CHECK(v.at(1) == 2);
    CHECK(v.at(2) == 1);
    CHECK(v.at(3) == 3);
}

TEST_CASE("Container helper: create vector of unique pointer") {
    std::vector<std::unique_ptr<int>> v =
            make_vector_of<std::unique_ptr<int>>(
                    std::unique_ptr<int>(new int(100)),
                    std::unique_ptr<int>(new int(200)));
    CHECK(v.size() == 2);
    CHECK(*v.at(0) == 100);
    CHECK(*v.at(1) == 200);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
