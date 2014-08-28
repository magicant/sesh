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
#include <memory>
#include <tuple>
#include "async/Delay.hh"
#include "common/Try.hh"
#include "common/copy.hh"
#include "common/type_tag.hh"

namespace {

using sesh::async::Delay;
using sesh::common::Try;
using sesh::common::copy;
using sesh::common::type_tag;

TEST_CASE("Delay: set result and callback") {
    using T = std::tuple<int, float, char>;
    Delay<T> s;

    s.setResult(type_tag<T>(), 42, 3.0f, 'a');

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

namespace forwarding {

TEST_CASE("Delay: simplest forward") {
    auto source = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(source), copy(target));

    CHECK(source.unique());
    source->setResult(42);
    source.reset();

    CHECK(target.unique());
    int result = 0;
    target->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 42);
}

TEST_CASE("Delay: forward with connected source") {
    auto source1 = std::make_shared<Delay<int>>();
    auto source2 = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(source1), copy(source2));
    Delay<int>::forward(copy(source2), copy(target));

    CHECK(source2.unique());
    source2.reset();

    CHECK(source1.unique());
    source1->setResult(42);
    source1.reset();

    CHECK(target.unique());
    int result = 0;
    target->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 42);
}

TEST_CASE("Delay: forward with abandoned source") {
    auto source1 = std::make_shared<Delay<int>>();
    auto source2 = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(source1), copy(source2));
    source1.reset();
    Delay<int>::forward(copy(source2), copy(target));

    CHECK(source2.unique());
    source2.reset();

    CHECK(target.unique());
    target->setCallback([](Try<int> &&) { FAIL("unexpected"); });
}

TEST_CASE("Delay: forward with connected target") {
    auto source = std::make_shared<Delay<int>>();
    auto target1 = std::make_shared<Delay<int>>();
    auto target2 = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(target1), copy(target2));
    Delay<int>::forward(copy(source), copy(target1));

    CHECK(target1.unique());
    target1.reset();

    CHECK(source.unique());
    source->setResult(42);
    source.reset();

    CHECK(target2.unique());
    int result = 0;
    target2->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 42);
}

TEST_CASE("Delay: forward with connected source and target") {
    auto source1 = std::make_shared<Delay<int>>();
    auto source2 = std::make_shared<Delay<int>>();
    auto target1 = std::make_shared<Delay<int>>();
    auto target2 = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(source1), copy(source2));
    Delay<int>::forward(copy(target1), copy(target2));
    Delay<int>::forward(copy(source2), copy(target1));

    CHECK(source2.unique());
    CHECK(target1.unique());
    source2.reset();
    target1.reset();

    CHECK(source1.unique());
    source1->setResult(42);
    source1.reset();

    CHECK(target2.unique());
    int result = 0;
    target2->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 42);
}

TEST_CASE("Delay: forward from source with preset result") {
    auto source = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    source->setResult(42);
    Delay<int>::forward(copy(source), copy(target));

    CHECK(source.unique());
    source.reset();

    CHECK(target.unique());
    int result = 0;
    target->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 42);
}

TEST_CASE("Delay: forward to target with preset callback") {
    auto source = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    int result = 0;
    target->setCallback([&result](Try<int> &&r) { result = *r; });
    Delay<int>::forward(copy(source), copy(target));

    CHECK(target.unique());
    target.reset();

    CHECK(source.unique());
    CHECK(result == 0);
    source->setResult(42);
    CHECK(result == 42);
}

TEST_CASE("Delay: forward preset result to preset callback") {
    auto source = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    source->setResult(42);
    int result = 0;
    target->setCallback([&result](Try<int> &&r) { result = *r; });
    CHECK(result == 0);
    Delay<int>::forward(copy(source), copy(target));
    CHECK(result == 42);

    CHECK(source.unique());
    source.reset();

    CHECK(target.unique());
}

TEST_CASE("Delay: uniqueness of target shared pointer after forward") {
    auto source = std::make_shared<Delay<int>>();
    auto target = std::make_shared<Delay<int>>();
    Delay<int>::forward(copy(source), copy(target));
    source.reset();
    CHECK(target.unique());
}

} // namespace forwarding

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
