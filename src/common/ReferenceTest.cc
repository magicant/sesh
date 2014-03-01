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

#ifndef INCLUDED_common_ReferenceTest_hh
#define INCLUDED_common_ReferenceTest_hh

#include "buildconfig.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common/Reference.hh"

namespace {

using sesh::common::Reference;

unsigned identity(unsigned long v) noexcept {
    return static_cast<unsigned>(v);
}

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable(NonCopyable &&) = default;
    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable &operator=(NonCopyable &&) = default;
};

TEST_CASE("Reference, construction") {
    int i;
    const Reference<int> r1 = i;
    Reference<int> r2 = r1;
    Reference<int> r3 = static_cast<Reference<int> &&>(r2);
    Reference<int> r4 = std::ref(i);
    (void) r3, (void) r4;
}

TEST_CASE("Reference, get and assignment") {
    int i1 = 111, i2 = 222;
    Reference<int> r1 = i1;
    const Reference<int> r2 = i2;
    CHECK(&r1.get() == &i1);
    CHECK(&r2.get() == &i2);

    r1 = r2;
    CHECK(&r1.get() == &i2);
    CHECK(&r2.get() == &i2);

    r1 = Reference<int>(i1);
    CHECK(&r1.get() == &i1);
    CHECK(&r2.get() == &i2);

    int &x1 = r1, &x2 = r2;
    CHECK(&x1 == &i1);
    CHECK(&x2 == &i2);
}

TEST_CASE("Reference, operator()") {
    Reference<unsigned(unsigned long)> r = identity;
    CHECK(r(42L) == 42);
}

TEST_CASE("Reference, movability") {
    struct Wrapper {
        NonCopyable nc;
        Reference<int> r;
        Wrapper(int &i) : r(i) { }
    };

    int i;
    Wrapper w1(i);
    Wrapper w2 = static_cast<Wrapper &&>(w1);
    w1 = static_cast<Wrapper &&>(w2);
}

} // namespace

#endif // #ifndef INCLUDED_common_ReferenceTest_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
