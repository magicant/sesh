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

#include <functional>
#include <memory>
#include "common/DirectInitialize.hh"
#include "common/SharedFunction.hh"

namespace {

using sesh::common::DIRECT_INITIALIZE;
using sesh::common::SharedFunction;
using sesh::common::makeSharedFunction;

template<typename T>
T id(T t) {
    return t;
}

class FunctionStub {

private:

    int &mInt;
    double &mDouble;

public:

    FunctionStub(int &i, double &d) noexcept : mInt(i), mDouble(d) { }

    FunctionStub(const FunctionStub &) = delete;
    FunctionStub(FunctionStub &&) = delete;
    FunctionStub &operator=(const FunctionStub &) = delete;
    FunctionStub &operator=(FunctionStub &&) = delete;
    ~FunctionStub() = default;

    char operator()(double d, int i) noexcept {
        mInt = i;
        mDouble = d;
        return 'A';
    }

}; // class FunctionStub

TEST_CASE("Shared function: direct initialization, assignment, and call") {
    int i = 0;
    double d = 1.0;
    const SharedFunction<FunctionStub> f1(DIRECT_INITIALIZE, i, d);
    const SharedFunction<FunctionStub> f2 = f1;
    CHECK(f2(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction with allocator") {
    int i = 0;
    double d = 1.0;
    const SharedFunction<FunctionStub> f(
            std::allocator_arg, std::allocator<FunctionStub>(), i, d);
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction from shared pointer") {
    int i = 0;
    double d = 1.0;
    const SharedFunction<FunctionStub> f(std::make_shared<FunctionStub>(i, d));
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction from const shared pointer") {
    int i = 0;
    double d = 1.0;
    const auto s = std::make_shared<FunctionStub>(i, d);
    const SharedFunction<FunctionStub> f(s);
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: simple function pointer") {
    const SharedFunction<int(*)(int)> sf(DIRECT_INITIALIZE, std::ref(id<int>));
    std::function<int(int)> f = sf;
    CHECK(f(2) == 2);
}

TEST_CASE("Shared function: insertion to std::function") {
    int i = 0;
    double d = 1.0;
    const std::function<char(double, int)> f(
            SharedFunction<FunctionStub>(DIRECT_INITIALIZE, i, d));
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: make shared function") {
    int (*pointer)(int) = id<int>;
    const SharedFunction<int(*const)(int)> &sf = makeSharedFunction(pointer);
    std::function<int(int)> f = sf;
    CHECK(f(2) == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
