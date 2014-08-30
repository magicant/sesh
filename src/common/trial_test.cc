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
#include "common/trial.hh"
#include "common/type_tag.hh"

namespace {

using sesh::common::trial;
using sesh::common::type_tag;

TEST_CASE("Trial, construction") {
    trial<int> r1(type_tag<int>(), 5);
    trial<int> r2((type_tag<std::exception_ptr>()));
    (void) r1, (void) r2;
}

TEST_CASE("Trial, has value") {
    trial<int> r1(type_tag<int>(), 5);
    trial<int> r2((type_tag<std::exception_ptr>()));
    CHECK(r1.has_value());
    CHECK_FALSE(r2.has_value());
}

TEST_CASE("Trial, conversion to bool") {
    trial<int> r1(type_tag<int>(), 5);
    trial<int> r2((type_tag<std::exception_ptr>()));
    CHECK(r1);
    CHECK_FALSE(r2);
}

TEST_CASE("Trial, operator*, value") {
    trial<int> r(type_tag<int>(), 42);
    CHECK(*r == 42);
    CHECK_NOTHROW(*r = 123);
    CHECK(*const_cast<const trial<int> &>(r) == 123);
}

TEST_CASE("Trial, operator*, exception") {
    class E {};
    trial<int> r(type_tag<std::exception_ptr>(), std::make_exception_ptr(E()));
    CHECK_THROWS_AS(*r, E);
    CHECK_THROWS_AS(*const_cast<const trial<int> &>(r), E);
}

TEST_CASE("Trial, operator->, value") {
    struct V {
        int get() noexcept { return 1; }
        int get() const noexcept { return 2; }
    };
    trial<V> r((type_tag<V>()));
    CHECK(r->get() == 1);
    CHECK(const_cast<const trial<V> &>(r)->get() == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
