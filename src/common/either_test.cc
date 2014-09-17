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

#include <exception>
#include <stdexcept>
#include <type_traits>
#include "catch.hpp"
#include "common/either.hh"
#include "common/empty.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace {

using sesh::common::bad_maybe_access;
using sesh::common::either;
using sesh::common::empty;
using sesh::common::type_tag;
using sesh::common::variant;

TEST_CASE("Either: is a subclass of variant") {
    using V = variant<int, double>;
    using E = either<int, double>;
    CHECK((std::is_base_of<V, E>::value));
}

TEST_CASE("Either: member types") {
    using E = either<int, double>;
    CHECK((std::is_same<E::error_type, int>::value));
    CHECK((std::is_same<E::value_type, double>::value));
    CHECK((std::is_same<E::reference, double &>::value));
    CHECK((std::is_same<E::const_reference, const double &>::value));
    CHECK((std::is_same<E::pointer, double *>::value));
    CHECK((std::is_same<E::const_pointer, const double *>::value));
}

TEST_CASE("Either: default construction") {
    either<int, double> e;
    REQUIRE(e.tag() == type_tag<int>());
    CHECK(e.value<int>() == int());
}

TEST_CASE("Either: in-place construction") {
    struct X {
        constexpr X(int, char, double) noexcept { }
    };
    either<int, X>(type_tag<X>(), 1, '1', 1.0);
}

TEST_CASE("Either: conversion to Boolean") {
    either<int, double> e1 = 1.0, e2 = 0;
    CHECK(static_cast<bool>(e1));
    CHECK_FALSE(static_cast<bool>(e2));
    CHECK(!e2);
}

TEST_CASE("Either: operator*") {
    either<int, double> e = 1.0;
    CHECK((std::is_same<decltype(*e), double &>::value));
    CHECK(*e == 1.0);

    const auto c = e;
    CHECK((std::is_same<decltype(*c), const double &>::value));
    CHECK(*c == 1.0);
}

TEST_CASE("Either: operator->") {
    struct X {
        double f() { return 1.0; }
        char f() const { return '1'; }
    };

    either<int, X> e = X();
    CHECK(e->f() == 1.0);

    const auto c = e;
    CHECK(c->f() == '1');
}

TEST_CASE("Either: value of non-empty maybe") {
    either<empty, int> e = 2;
    CHECK_NOTHROW(e.get() = 3);

    const auto c = e;
    CHECK(c.get() == 3);
}

TEST_CASE("Either: value of empty maybe") {
    either<empty, int> e;
    CHECK_THROWS_AS(e.get(), bad_maybe_access);

    const auto c = e;
    CHECK_THROWS_AS(c.get(), bad_maybe_access);
}

TEST_CASE("Either: value of successful trial") {
    either<std::exception_ptr, int> e = 2;
    CHECK_NOTHROW(e.get() = 3);

    const auto c = e;
    CHECK(c.get() == 3);
}

TEST_CASE("Either: value of default failed trial") {
    either<std::exception_ptr, int> e;
    CHECK_THROWS_AS(e.get(), std::logic_error);

    const auto c = e;
    CHECK_THROWS_AS(c.get(), std::logic_error);
}

TEST_CASE("Either: value of explicitly failed trial") {
    struct X { };

    either<std::exception_ptr, int> e(std::make_exception_ptr(X()));
    CHECK_THROWS_AS(e.get(), X);

    const auto c = e;
    CHECK_THROWS_AS(c.get(), X);
}

TEST_CASE("Either: value_or with successful result") {
    either<int, double> e = 1.0;
    double d = 2.0;
    CHECK((std::is_same<decltype(e.value_or(d)), double &>::value));
    CHECK(e.value_or(d) == 1.0);

    const auto c = e;
    CHECK((std::is_same<decltype(c.value_or(d)), const double &>::value));
    CHECK(c.value_or(d) == 1.0);

    CHECK((std::is_same<
                decltype(std::move(e).value_or(d)), const double &>::value));
    CHECK(std::move(e).value_or(d) == 1.0);
}

TEST_CASE("Either: value_or with failed result") {
    either<int, double> e = 0;
    double d = 2.0;
    CHECK(&e.value_or(d) == &d);

    const auto c = e;
    CHECK(&c.value_or(d) == &d);

    double &&d2 = std::move(e).value_or(std::move(d));
    CHECK(&d2 == &d);
}

TEST_CASE("Either: successful emplacement on maybe") {
    either<empty, double> e = 1.0;
    CHECK_NOTHROW(e.try_emplace(2.0));
    CHECK(e.get() == 2.0);
}

TEST_CASE("Either: successful emplacement on trial") {
    either<std::exception_ptr, double> e = 1.0;
    CHECK_NOTHROW(e.try_emplace(2.0));
    CHECK(e.get() == 2.0);
}

struct failer {
    failer(int, double, char) { throw empty(); }
};

TEST_CASE("Either: failed emplacement on maybe") {
    either<empty, failer> e;
    CHECK_THROWS_AS(e.try_emplace(1, 2.0, '3'), empty);
    CHECK(e.tag() == type_tag<empty>());
}

TEST_CASE("Either: failed emplacement on trial") {
    either<std::exception_ptr, failer> e;
    CHECK_THROWS_AS(e.try_emplace(1, 2.0, '3'), empty);
    REQUIRE(e.tag() == type_tag<std::exception_ptr>());
    CHECK_FALSE(e.value<std::exception_ptr>());
}

TEST_CASE("Either: clear from successful result") {
    either<int, double> e = 1.0;
    e.clear();
    REQUIRE(e.tag() == type_tag<int>());
    CHECK(e.value<int>() == int());
}

TEST_CASE("Either: clear from failed result") {
    either<int, double> e = 2;
    e.clear();
    REQUIRE(e.tag() == type_tag<int>());
    CHECK(e.value<int>() == int());
}

TEST_CASE("Either: operator= with value type, success") {
    either<int, double> e = 1.0;
    CHECK_NOTHROW((e = 2) = 3);
    REQUIRE(e.tag() == type_tag<double>());
    CHECK(e.value<double>() == 3.0);
}

struct copy_thrower {
    copy_thrower() = default;
    copy_thrower(const copy_thrower &) { throw empty(); }
};

TEST_CASE("Either: operator= with value type, failure") {
    either<empty, copy_thrower> e;
    CHECK_THROWS_AS(e = copy_thrower(), empty);
    CHECK(e.tag() == type_tag<empty>());
}

TEST_CASE("Either: operator= with another either, success") {
    using E = either<int, double>;
    E e = 1;

    e = E(2);
    REQUIRE(e.tag() == type_tag<int>());
    CHECK(e.value<int>() == 2);

    e = E(1.0);
    REQUIRE(e.tag() == type_tag<double>());
    CHECK(e.value<double>() == 1.0);

    e = E(2.0);
    REQUIRE(e.tag() == type_tag<double>());
    CHECK(e.value<double>() == 2.0);

    e = E(1);
    REQUIRE(e.tag() == type_tag<int>());
    CHECK(e.value<int>() == 1);
}

TEST_CASE("Either: operator= with another either, failure") {
    using E = either<empty, copy_thrower>;
    E e;
    CHECK_THROWS_AS(e = E(type_tag<copy_thrower>()), empty);
    CHECK(e.tag() == type_tag<empty>());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
