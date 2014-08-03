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
#include <tuple>
#include "async/Delay.hh"
#include "common/Try.hh"
#include "common/TypeTag.hh"

namespace {

using sesh::async::Delay;
using sesh::common::Try;
using sesh::common::TypeTag;

TEST_CASE("Delay: set result and callback") {
    using T = std::tuple<int, float, char>;
    Delay<T> s;

    s.setResult(TypeTag<T>(), 42, 3.0f, 'a');

    unsigned callCount = 0;
    s.setCallback([&callCount](Try<T> &&r) {
        ++callCount;
        CHECK(*r == std::make_tuple(42, 3.0f, 'a'));
    });
    CHECK(callCount == 1);
}

TEST_CASE("Delay: set callback and result") {
    Delay<int> s;

    unsigned callCount = 0;
    s.setCallback([&callCount](Try<int> &&r) {
        CHECK(*r == 42);
        ++callCount;
    });
    CHECK(callCount == 0);

    s.setResult(42);
    CHECK(callCount == 1);
}

TEST_CASE("Delay: set result with throwing constructor and then callback") {
    class Thrower {
    public:
        Thrower() noexcept = default;
        Thrower(const Thrower &) { throw 42; }
    };

    Delay<Thrower> s;

    s.setResult(Thrower());

    unsigned callCount = 0;
    s.setCallback([&callCount](Try<Thrower> &&r) {
        ++callCount;
        try {
            *r;
        } catch (int i) {
            CHECK(i == 42);
        }
    });
    CHECK(callCount == 1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
