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

#include <exception>
#include "async/Delay.hh"
#include "async/Result.hh"

namespace {

using sesh::async::Delay;
using sesh::async::Result;

TEST_CASE("Delay, set successful result and callback") {
    Delay<int> s;

    s.setResultFrom([] { return 42; });

    unsigned callCount = 0;
    s.setCallback([&callCount](Result<int> &&r) {
        ++callCount;
        CHECK(*r == 42);
    });
    CHECK(callCount == 1);
}

TEST_CASE("Delay, set callback and successful result") {
    Delay<int> s;

    unsigned callCount = 0;
    s.setCallback([&callCount](Result<int> &&r) {
        CHECK(*r == 42);
        ++callCount;
    });
    CHECK(callCount == 0);

    s.setResultFrom([] { return 42; });
    CHECK(callCount == 1);
}

TEST_CASE("Delay, set exception and callback") {
    Delay<int> s;

    s.setResultFrom([]() -> int { throw 42; });

    unsigned callCount = 0;
    s.setCallback([&callCount](Result<int> &&r) {
        ++callCount;
        try {
            *r;
        } catch (int i) {
            CHECK(i == 42);
        }
    });
    CHECK(callCount == 1);
}

TEST_CASE("Delay, set callback and exception") {
    Delay<int> s;

    unsigned callCount = 0;
    s.setCallback([&callCount](Result<int> &&r) {
        try {
            *r;
        } catch (int i) {
            CHECK(i == 42);
            ++callCount;
        }
    });
    CHECK(callCount == 0);

    s.setResultFrom([]() -> int { throw 42; });
    CHECK(callCount == 1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
