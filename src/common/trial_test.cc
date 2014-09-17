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
#include "catch.hpp"
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

TEST_CASE("Trial, conversion to bool") {
    trial<int> r1(type_tag<int>(), 5);
    trial<int> r2((type_tag<std::exception_ptr>()));
    CHECK(r1);
    CHECK_FALSE(r2);
}

TEST_CASE("Trial, operator*, value") {
    trial<int> r(type_tag<int>(), 42);
    CHECK(r.get() == 42);
    CHECK_NOTHROW(r.get() = 123);
    CHECK(const_cast<const trial<int> &>(r).get() == 123);
}

TEST_CASE("Trial, operator*, exception") {
    class E {};
    trial<int> r(type_tag<std::exception_ptr>(), std::make_exception_ptr(E()));
    CHECK_THROWS_AS(r.get(), E);
    CHECK_THROWS_AS(const_cast<const trial<int> &>(r).get(), E);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
