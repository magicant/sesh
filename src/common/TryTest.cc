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
#include "common/Try.hh"
#include "common/type_tag.hh"

namespace {

using sesh::common::Try;
using sesh::common::type_tag;

TEST_CASE("Try, construction") {
    Try<int> r1(type_tag<int>(), 5);
    Try<int> r2((type_tag<std::exception_ptr>()));
    (void) r1, (void) r2;
}

TEST_CASE("Try, has value") {
    Try<int> r1(type_tag<int>(), 5);
    Try<int> r2((type_tag<std::exception_ptr>()));
    CHECK(r1.hasValue());
    CHECK_FALSE(r2.hasValue());
}

TEST_CASE("Try, conversion to bool") {
    Try<int> r1(type_tag<int>(), 5);
    Try<int> r2((type_tag<std::exception_ptr>()));
    CHECK(r1);
    CHECK_FALSE(r2);
}

TEST_CASE("Try, operator*, value") {
    Try<int> r(type_tag<int>(), 42);
    CHECK(*r == 42);
    CHECK_NOTHROW(*r = 123);
    CHECK(*const_cast<const Try<int> &>(r) == 123);
}

TEST_CASE("Try, operator*, exception") {
    class E {};
    Try<int> r(type_tag<std::exception_ptr>(), std::make_exception_ptr(E()));
    CHECK_THROWS_AS(*r, E);
    CHECK_THROWS_AS(*const_cast<const Try<int> &>(r), E);
}

TEST_CASE("Try, operator->, value") {
    struct V {
        int get() noexcept { return 1; }
        int get() const noexcept { return 2; }
    };
    Try<V> r((type_tag<V>()));
    CHECK(r->get() == 1);
    CHECK(const_cast<const Try<V> &>(r)->get() == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
