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

#include <type_traits>
#include "common/function_helper.hh"

namespace {

using sesh::common::is_callable;
using sesh::common::result_of_t;

class dummy {
public:
    void operator()(int, double);
    void operator()(double, long);
};

class stub {
public:
    bool b;
    char operator()(int, double);
    long operator()(double, long);
    dummy operator*();
};

} // namespace

template<typename Callable, typename... Argument>
auto result_of_sfinae(Callable &&c, Argument &&... arg)
    -> decltype(
            std::declval<result_of_t<Callable(Argument...)>>(),
            std::true_type());

std::false_type result_of_sfinae(...);

namespace {

TEST_CASE("Functional object is callable") {
    CHECK(is_callable<stub(int, const double &)>::value);
}

TEST_CASE("Functional object is not callable with invalid argument types") {
    CHECK_FALSE(is_callable<stub(double, double)>::value);
}

TEST_CASE("Result of well-typed function call") {
    using R = result_of_t<stub(double, int)>;
    CHECK((std::is_same<R, long>::value));
}

TEST_CASE("Result of ill-typed function call") {
    using R = decltype(result_of_sfinae(stub(), 1.0, 1.0));
    CHECK_FALSE(R::value);
}

TEST_CASE("Member function pointer is callable") {
    using P = char (stub::*)(int, double);
    CHECK(is_callable<P(stub, int, double)>::value);
    CHECK(is_callable<P(stub *, int, double)>::value);
}

TEST_CASE("Member function pointer is not callable without argument") {
    using P = char (stub::*)(int);
    CHECK_FALSE(is_callable<P(stub)>::value);
    CHECK_FALSE(is_callable<P(stub *)>::value);
}

TEST_CASE("Result of well-typed member function pointer call") {
    using P = char (stub::*)(int, double);
    using R1 = result_of_t<P(stub, int, double)>;
    CHECK((std::is_same<R1, char>::value));
    using R2 = result_of_t<P(stub *, int, double)>;
    CHECK((std::is_same<R2, char>::value));
}

TEST_CASE("Result of member function pointer call without arguments") {
    using P = char (stub::*)(int, double);
    CHECK_FALSE(decltype(result_of_sfinae(std::declval<P>(), stub()))::value);
    CHECK_FALSE(decltype(result_of_sfinae(
            std::declval<P>(), std::declval<stub *>()))::value);
}

TEST_CASE("Data member pointer is callable") {
    using P = bool stub::*;
    CHECK(is_callable<P(const stub &)>::value);
    CHECK(is_callable<P(const stub *)>::value);
}

TEST_CASE("Data member pointer is not callable with bogus arguments") {
    using P = bool stub::*;
    CHECK_FALSE(is_callable<P(const stub &, int, double)>::value);
    CHECK_FALSE(is_callable<P(const stub *, int, double)>::value);
}

TEST_CASE("Result of well-typed data member pointer dereference") {
    using P = bool stub::*;
    CHECK((std::is_same<result_of_t<P(stub)>, bool &&>::value));
    CHECK((std::is_same<result_of_t<P(stub *)>, bool &>::value));
}

TEST_CASE("Result of data member pointer dereference with bogus arguments") {
    using P = bool stub::*;
    using R1 = decltype(result_of_sfinae(std::declval<P>(), stub(), 1));
    CHECK_FALSE(R1::value);
    using R2 = decltype(
            result_of_sfinae(std::declval<P>(), std::declval<stub *>(), 1));
    CHECK_FALSE(R2::value);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
