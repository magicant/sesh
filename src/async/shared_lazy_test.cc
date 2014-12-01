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

#include <utility>
#include "catch.hpp"
#include "async/shared_lazy.hh"

namespace {

using sesh::async::lazy;
using sesh::async::shared_lazy;

struct move_only {
    move_only() = default;
    move_only(move_only &&) = default;
    move_only &operator=(move_only &&) = default;
};

TEST_CASE("Default-constructed shared lazy") {
    shared_lazy<int> l;
    CHECK(l->get() == int());
}

TEST_CASE("Default-constructed shared lazy (const)") {
    shared_lazy<const int> l;
    CHECK(l->get() == int());
}

TEST_CASE("Shared lazy with already computed value") {
    shared_lazy<int> l = 3;
    CHECK(l->get() == 3);
}

TEST_CASE("Shared lazy with already computed value (r-value)") {
    shared_lazy<move_only> l = move_only();
    CHECK(*l);
}

TEST_CASE("Shared lazy with already computed value (const)") {
    shared_lazy<const int> l = 3;
    CHECK(l->get() == 3);
}

TEST_CASE("Shared lazy with existing shared pointer") {
    const auto p = std::make_shared<const lazy<int>>();
    shared_lazy<const int> l(p, *p);
    (void) *l;
    CHECK((**p).get() == int());
}

TEST_CASE("Shared lazy value computation") {
    const shared_lazy<int> l([] { return 42; });
    CHECK(l->get() == 42);
    CHECK(l->get() == 42);
}

TEST_CASE("Shared lazy value is computed only once") {
    bool called = false;
    shared_lazy<int> l([&called] {
        CHECK_FALSE(called);
        called = true;
        return 0;
    });
    (void) *l;
    CHECK(called);
    (void) *l;
}

TEST_CASE("Copied shared lazies share the same value") {
    const shared_lazy<int> l1([] { return 42; });
    const shared_lazy<int> l2 = l1;
    CHECK(l2->get() == 42);
    l2->get() = 1;
    CHECK(l1->get() == 1);
}

TEST_CASE("R-value shared lazy is copied") {
    shared_lazy<int> l1([] { return 42; });
    shared_lazy<int> l2 = std::move(l1);
    l2 = std::move(l1);
    CHECK(l2->get() == 42);
    l2->get() = 1;
    CHECK(l1->get() == 1);
}

TEST_CASE("shared_lazy::has_computed") {
    shared_lazy<int> l([&l] { CHECK_FALSE(l.has_computed()); return 3; });
    CHECK_FALSE(l.has_computed());
    (void) *l;
    CHECK(l.has_computed());
}

TEST_CASE("shared_lazy::is_computing") {
    shared_lazy<int> l([&l] { CHECK(l.is_computing()); return 3; });
    CHECK_FALSE(l.is_computing());
    (void) *l;
    CHECK_FALSE(l.is_computing());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
