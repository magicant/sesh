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

#include "catch.hpp"
#include "common/reference.hh"

namespace {

using sesh::common::reference;

unsigned identity(unsigned long v) noexcept {
    return static_cast<unsigned>(v);
}

struct non_copyable {
    non_copyable() = default;
    non_copyable(const non_copyable &) = delete;
    non_copyable(non_copyable &&) = default;
    non_copyable &operator=(const non_copyable &) = delete;
    non_copyable &operator=(non_copyable &&) = default;
};

TEST_CASE("Reference, construction") {
    int i;
    const reference<int> r1 = i;
    reference<int> r2 = r1;
    reference<int> r3 = static_cast<reference<int> &&>(r2);
    reference<int> r4 = std::ref(i);
    (void) r3, (void) r4;
}

TEST_CASE("Reference, get and assignment") {
    int i1 = 111, i2 = 222;
    reference<int> r1 = i1;
    const reference<int> r2 = i2;
    CHECK(&r1.get() == &i1);
    CHECK(&r2.get() == &i2);

    r1 = r2;
    CHECK(&r1.get() == &i2);
    CHECK(&r2.get() == &i2);

    r1 = reference<int>(i1);
    CHECK(&r1.get() == &i1);
    CHECK(&r2.get() == &i2);

    int &x1 = r1, &x2 = r2;
    CHECK(&x1 == &i1);
    CHECK(&x2 == &i2);
}

TEST_CASE("Reference, operator()") {
    reference<unsigned(unsigned long)> r = identity;
    CHECK(r(42L) == 42);
}

TEST_CASE("Reference, movability") {
    struct wrapper {
        non_copyable nc;
        reference<int> r;
        wrapper(int &i) : r(i) { }
    };

    int i;
    wrapper w1(i);
    wrapper w2 = static_cast<wrapper &&>(w1);
    w1 = static_cast<wrapper &&>(w2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
