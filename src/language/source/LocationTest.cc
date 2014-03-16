/* Copyright (C) 2013 WATANABE Yuki
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

#include <memory>
#include <stdexcept>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/Location.hh"
#include "language/source/LocationTestHelper.hh"

namespace {

using sesh::common::String;
using sesh::language::source::Location;
using sesh::language::source::dummyLocation;

TEST_CASE("Location, construction") {
    std::shared_ptr<const String> name = std::make_shared<String>(L("12"));

    Location l1(name, 0, 0);
    Location l2(decltype(name)(name), 1, 1);

    name = std::make_shared<String>(L("34"));

    Location l3(name, 3, 4);
    Location l4(decltype(name)(name), 1234, 9876);

    CHECK(l1.name() == L("12"));
    CHECK(l1.line() == 0);
    CHECK(l1.column() == 0);
    CHECK(l2.name() == L("12"));
    CHECK(l2.line() == 1);
    CHECK(l2.column() == 1);
    CHECK(l3.name() == L("34"));
    CHECK(l3.line() == 3);
    CHECK(l3.column() == 4);
    CHECK(l4.name() == L("34"));
    CHECK(l4.line() == 1234);
    CHECK(l4.column() == 9876);

    l1 = l2;
    CHECK(l1.line() == 1);
    l1 = std::move(l3);
    CHECK(l1.line() == 3);
}

TEST_CASE("Location, constructor null pointer exception") {
    std::shared_ptr<const String> name(nullptr);
    CHECK_THROWS_AS(Location(name, 0, 0), std::invalid_argument);
    CHECK_THROWS_AS(Location(std::move(name), 0, 0), std::invalid_argument);
}

TEST_CASE("Location, comparison") {
    Location l1 = dummyLocation(L("apple"), 0, 0);
    Location l2 = dummyLocation(L("apple"), 0, 0);
    Location l3 = dummyLocation(L("banana"), 0, 0);
    Location l4 = dummyLocation(L("apple"), 1, 0);
    Location l5 = dummyLocation(L("apple"), 0, 1);

    CHECK(l1 == l1);
    CHECK_FALSE(!(l1 == l1));
    CHECK(l1 == l2);
    CHECK_FALSE(!(l1 == l2));
    CHECK_FALSE(l1 == l3);
    CHECK(!(l1 == l3));
    CHECK_FALSE(l1 == l4);
    CHECK(!(l1 == l4));
    CHECK_FALSE(l1 == l5);
    CHECK(!(l1 == l5));
    CHECK_FALSE(l3 == l4);
    CHECK(!(l3 == l4));
    CHECK_FALSE(l3 == l5);
    CHECK(!(l3 == l5));
}

TEST_CASE("Dummy source location") {
    Location l1 = dummyLocation();
    Location l2 = dummyLocation(L("foo"));
    Location l3 = dummyLocation(L("foo"), 123);
    Location l4 = dummyLocation(L("foo"), 123, 456);

    CHECK(l1.name() == L("dummy"));
    CHECK(l1.line() == 1);
    CHECK(l1.column() == 1);
    CHECK(l2.name() == L("foo"));
    CHECK(l2.line() == 1);
    CHECK(l2.column() == 1);
    CHECK(l3.name() == L("foo"));
    CHECK(l3.line() == 123);
    CHECK(l3.column() == 1);
    CHECK(l4.name() == L("foo"));
    CHECK(l4.line() == 123);
    CHECK(l4.column() == 456);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
