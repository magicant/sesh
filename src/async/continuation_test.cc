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

#include <memory>
#include <type_traits>
#include "async/continuation.hh"
#include "catch.hpp"

namespace {

using sesh::async::continuation;
using sesh::async::runnable_wrapper;

TEST_CASE("Continuation special member function properties") {
    CHECK(std::is_default_constructible<continuation>::value);
    CHECK_FALSE(std::is_copy_constructible<continuation>::value);
    CHECK(std::is_move_constructible<continuation>::value);
    CHECK_FALSE(std::is_copy_assignable<continuation>::value);
    CHECK(std::is_move_assignable<continuation>::value);
    CHECK(std::is_destructible<continuation>::value);
}

TEST_CASE("Null-constructed continuation does nothing on destruction") {
    (void) continuation(nullptr);
}

TEST_CASE("continuation::run") {
    int i = 0;
    auto f = [&i] { CHECK(i == 0); i = 100; };
    continuation c(std::make_shared<runnable_wrapper<decltype(f)>>(f));
    CHECK(i == 0);
    c.run();
    CHECK(i == 100);
    c.run();
    CHECK(i == 100);
}

TEST_CASE("Continuation returned by runnable is run in turn") {
    int i = 0;
    auto f = [&i] { CHECK(i == 0); i = 100; };
    auto g = [f] {
        return continuation(
                std::make_shared<runnable_wrapper<decltype(f)>>(f));
    };
    continuation c(std::make_shared<runnable_wrapper<decltype(g)>>(g));
    c.run();
    CHECK(i == 100);
}

TEST_CASE("Continuation destructor runs the continuation") {
    int i = 0;
    auto f = [&i] { CHECK(i == 0); i = 100; };
    (void) continuation(std::make_shared<runnable_wrapper<decltype(f)>>(f));
    CHECK(i == 100);
}

TEST_CASE("Continuation move construction") {
    int i = 0;
    auto f = [&i] { CHECK(i == 0); i = 100; };
    auto c1 = continuation(std::make_shared<runnable_wrapper<decltype(f)>>(f));
    {
        auto c2 = continuation(std::move(c1));
        CHECK(i == 0);
    }
    CHECK(i == 100);
}

TEST_CASE("Swapping continuations") {
    int i1 = 0, i2 = 0;
    auto f1 = [&i1] { CHECK(i1 == 0); i1 = 100; };
    auto f2 = [&i2] { CHECK(i2 == 0); i2 = 200; };
    {
        continuation c1(std::make_shared<runnable_wrapper<decltype(f1)>>(f1));
        {
            continuation c2(
                    std::make_shared<runnable_wrapper<decltype(f2)>>(f2));
            c1.swap(c2);
            CHECK(i1 == 0);
            CHECK(i2 == 0);
        }
        CHECK(i1 == 100);
        CHECK(i2 == 0);
    }
    CHECK(i1 == 100);
    CHECK(i2 == 200);
}

TEST_CASE("Move assignment swaps continuations") {
    int i1 = 0, i2 = 0;
    auto f1 = [&i1] { CHECK(i1 == 0); i1 = 100; };
    auto f2 = [&i2] { CHECK(i2 == 0); i2 = 200; };
    {
        continuation c1(std::make_shared<runnable_wrapper<decltype(f1)>>(f1));
        {
            continuation c2(
                    std::make_shared<runnable_wrapper<decltype(f2)>>(f2));
            c1 = std::move(c2);
            CHECK(i1 == 0);
            CHECK(i2 == 0);
        }
        CHECK(i1 == 100);
        CHECK(i2 == 0);
    }
    CHECK(i1 == 100);
    CHECK(i2 == 200);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
