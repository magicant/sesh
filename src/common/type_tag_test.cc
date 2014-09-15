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

#include <type_traits>
#include "catch.hpp"
#include "common/type_tag.hh"

namespace {

using sesh::common::type_tag;

constexpr type_tag<int> INT{};
constexpr type_tag<char> CHAR{};
constexpr type_tag<float> FLOAT{};
constexpr type_tag<type_tag<int>> INT_TAG{};

static_assert(
        sizeof(type_tag<>) > 0,
        "empty type tag is a valid type, if not constructible");

TEST_CASE("Type tag: comparison of single-type tags") {
    CHECK(INT == INT);
    CHECK_FALSE(INT < INT);
}

TEST_CASE("Type tag: conversion from single-type tag to enumeration") {
    switch (INT) {
    case INT:
        break;
    default:
        FAIL();
        break;
    }
}

TEST_CASE("Type tag: application with single-type tag") {
    bool called = false;
    auto i = INT.apply([&called](type_tag<int>) { return called = true, 42; });
    static_assert(
            std::is_same<decltype(i), int>::value, "apply should return int");
    CHECK(i == 42);
    CHECK(called);
}

namespace double_type_tag {

using tag = type_tag<int, char>;
constexpr tag INT2 = INT;
constexpr tag CHAR2 = CHAR;

TEST_CASE("Type tag: equality of double-type tags") {
    CHECK(INT2 == INT2);
    CHECK(CHAR2 == CHAR2);
    CHECK_FALSE(INT2 == CHAR2);
    CHECK_FALSE(CHAR2 == INT2);
}

TEST_CASE("Type tag: equality between single- and double-type tags") {
    CHECK(INT2 == INT);
    CHECK(INT == INT2);
    CHECK_FALSE(INT2 == CHAR);
    CHECK_FALSE(CHAR == INT2);
}

TEST_CASE("Type tag: total order on double-type tags") {
    CHECK_FALSE(INT2 < INT2);
    CHECK(INT2 < CHAR2);
    CHECK_FALSE(CHAR2 < INT2);
    CHECK_FALSE(CHAR2 < CHAR2);
}

TEST_CASE("Type tag: total order on single- and double-type tags") {
    CHECK_FALSE(INT < INT2);
    CHECK(INT < CHAR2);
    CHECK_FALSE(CHAR < INT2);
    CHECK_FALSE(CHAR < CHAR2);
    CHECK_FALSE(INT2 < INT);
    CHECK(INT2 < CHAR);
    CHECK_FALSE(CHAR2 < INT);
    CHECK_FALSE(CHAR2 < CHAR);
}

TEST_CASE("Type tag: conversion from double-type tag to enumeration") {
    switch (INT2) {
    case INT2:
        break;
    case CHAR2:
        FAIL("CHAR2");
        break;
    default:
        FAIL("default");
        break;
    }
    switch (CHAR2) {
    case INT2:
        FAIL("INT2");
        break;
    case CHAR2:
        break;
    default:
        FAIL("default");
        break;
    }
}

namespace apply {

struct F {
    bool &int_called, &char_called;
    void operator()(type_tag<int>) { int_called = true; }
    void operator()(const type_tag<char> &) { char_called = true; }
};

TEST_CASE("Type tag: application with double-type tag of int") {
    bool int_called = false, char_called = false;
    INT2.apply(F{int_called, char_called});
    CHECK(int_called);
    CHECK_FALSE(char_called);
}

TEST_CASE("Type tag: application with double-type tag of char") {
    bool int_called = false, char_called = false;
    CHAR2.apply(F{int_called, char_called});
    CHECK_FALSE(int_called);
    CHECK(char_called);
}

} // namespace apply

} // namespace double_type_tag

namespace quad_type_tag {

using Tag = type_tag<int, char, float, type_tag<int>>;
constexpr Tag INT4 = INT;
constexpr Tag CHAR4 = CHAR;
constexpr Tag FLOAT4 = FLOAT;
constexpr Tag INT_TAG4 = INT_TAG;

TEST_CASE("Type tag: conversion from double- to quad-type tag") {
    Tag i = type_tag<int, char>(INT);
    Tag f = type_tag<char, float, int>(FLOAT);
    Tag c = type_tag<float, int, type_tag<int>, char>(CHAR);
    CHECK(i == INT4);
    CHECK(f == FLOAT4);
    CHECK(c == CHAR4);
}

TEST_CASE("Type tag: equality of quad-type tags") {
    CHECK(INT4 == INT4);
    CHECK(CHAR4 == CHAR4);
    CHECK(FLOAT4 == FLOAT4);
    CHECK(INT_TAG4 == INT_TAG4);
    CHECK_FALSE(INT4 == CHAR4);
    CHECK_FALSE(INT4 == INT_TAG4);
    CHECK_FALSE(CHAR4 == FLOAT4);
    CHECK_FALSE(INT_TAG4 == INT4);
    CHECK_FALSE(INT_TAG4 == FLOAT4);
}

TEST_CASE("Type tag: equality between single- and quad-type tags") {
    CHECK(CHAR4 == CHAR);
    CHECK(FLOAT == FLOAT4);
    CHECK_FALSE(INT4 == CHAR);
    CHECK_FALSE(FLOAT == INT_TAG4);
}

TEST_CASE("Type tag: total order on quad-type tags") {
    CHECK_FALSE(INT4 < INT4);
    CHECK(INT4 < CHAR4);
    CHECK(INT4 < FLOAT4);
    CHECK(INT4 < INT_TAG4);
    CHECK_FALSE(CHAR4 < INT4);
    CHECK_FALSE(CHAR4 < CHAR4);
    CHECK(CHAR4 < FLOAT4);
    CHECK(CHAR4 < INT_TAG4);
    CHECK_FALSE(FLOAT4 < INT4);
    CHECK_FALSE(FLOAT4 < CHAR4);
    CHECK_FALSE(FLOAT4 < FLOAT4);
    CHECK(FLOAT4 < INT_TAG4);
    CHECK_FALSE(INT_TAG4 < INT4);
    CHECK_FALSE(INT_TAG4 < CHAR4);
    CHECK_FALSE(INT_TAG4 < FLOAT4);
    CHECK_FALSE(INT_TAG4 < INT_TAG4);
}

TEST_CASE("Type tag: total order on single- and quad-type tags") {
    CHECK_FALSE(FLOAT < FLOAT4);
    CHECK_FALSE(FLOAT4 < FLOAT);
    CHECK(CHAR < FLOAT4);
    CHECK(CHAR4 < FLOAT);
    CHECK_FALSE(INT_TAG < FLOAT4);
    CHECK_FALSE(INT_TAG4 < FLOAT);
}

TEST_CASE("Type tag: conversion from quad-type tag to enumeration") {
    switch (FLOAT4) {
    case INT4:
        FAIL("INT4");
        break;
    case CHAR4:
        FAIL("CHAR4");
        break;
    case FLOAT4:
        break;
    case INT_TAG4:
        FAIL("INT_TAG4");
        break;
    default:
        FAIL("default");
        break;
    }
}

namespace apply_behavior {

template<typename T>
struct type_tag_acceptor {
    int operator()(type_tag<T>) { return 42; }
    template<typename U> int operator()(type_tag<U>) { FAIL(); return 0; }
};

TEST_CASE("Type tag: application with quad-type tag") {
    CHECK(INT4.apply(type_tag_acceptor<int>()) == 42);
    CHECK(CHAR4.apply(type_tag_acceptor<char>()) == 42);
    CHECK(FLOAT4.apply(type_tag_acceptor<float>()) == 42);
    CHECK(INT_TAG4.apply(type_tag_acceptor<type_tag<int>>()) == 42);
}

} // namespace apply_behavior

namespace apply_throw {

struct no_throw {
    template<typename T> void operator()(type_tag<T>) noexcept { }
};

struct throwing {
    void operator()(type_tag<type_tag<int>>) { }
    template<typename T> void operator()(type_tag<T>) noexcept { }
};

TEST_CASE("Type tag: exception specification of apply") {
    CHECK(noexcept(FLOAT4.apply(no_throw())));
    CHECK_FALSE(noexcept(FLOAT4.apply(throwing())));
}

} // namespace apply_throw

} // namespace quad_type_tag

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
