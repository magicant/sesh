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

#include <functional>
#include <memory>
#include "catch.hpp"
#include "common/direct_initialize.hh"
#include "common/shared_function.hh"

namespace {

using sesh::common::direct_initialize;
using sesh::common::make_shared_function;
using sesh::common::shared_function;

template<typename T>
T id(T t) {
    return t;
}

class function_stub {

private:

    int &m_int;
    double &m_double;

public:

    function_stub(int &i, double &d) noexcept : m_int(i), m_double(d) { }

    function_stub(const function_stub &) = delete;
    function_stub(function_stub &&) = delete;
    function_stub &operator=(const function_stub &) = delete;
    function_stub &operator=(function_stub &&) = delete;
    ~function_stub() = default;

    char operator()(double d, int i) noexcept {
        m_int = i;
        m_double = d;
        return 'A';
    }

}; // class function_stub

TEST_CASE("Shared function: direct initialization, assignment, and call") {
    int i = 0;
    double d = 1.0;
    const shared_function<function_stub> f1(direct_initialize(), i, d);
    const shared_function<function_stub> f2 = f1;
    CHECK(f2(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction with allocator") {
    int i = 0;
    double d = 1.0;
    const shared_function<function_stub> f(
            std::allocator_arg, std::allocator<function_stub>(), i, d);
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction from shared pointer") {
    int i = 0;
    double d = 1.0;
    const shared_function<function_stub> f(
            std::make_shared<function_stub>(i, d));
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: construction from const shared pointer") {
    int i = 0;
    double d = 1.0;
    const auto s = std::make_shared<function_stub>(i, d);
    const shared_function<function_stub> f(s);
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: simple function pointer") {
    using SF = shared_function<int(*)(int)>;
    const SF sf(direct_initialize(), std::ref(id<int>));
    std::function<int(int)> f = sf;
    CHECK(f(2) == 2);
}

TEST_CASE("Shared function: insertion to std::function") {
    int i = 0;
    double d = 1.0;
    const std::function<char(double, int)> f(
            shared_function<function_stub>(direct_initialize(), i, d));
    CHECK(f(0.0, 42) == 'A');
    CHECK(i == 42);
    CHECK(d == 0.0);
}

TEST_CASE("Shared function: create") {
    int i = 0;
    double d = 1.0;
    shared_function<function_stub> f =
            shared_function<function_stub>::create(i, d);
    CHECK(f(0.0, 42) == 'A');
}

TEST_CASE("Shared function: make shared function") {
    int (*pointer)(int) = id<int>;
    const shared_function<int(*const)(int)> &sf =
            make_shared_function(pointer);
    std::function<int(int)> f = sf;
    CHECK(f(2) == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
