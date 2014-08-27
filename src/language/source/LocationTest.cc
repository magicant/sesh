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
#include "common/String.hh"
#include "language/source/Location.hh"
#include "language/source/LocationTestHelper.hh"
#include "language/source/OriginTestHelper.hh"

namespace {

using sesh::common::String;
using sesh::language::source::LineLocation;
using sesh::language::source::Location;
using sesh::language::source::Origin;
using sesh::language::source::dummyLineLocation;
using sesh::language::source::dummyLocation;
using sesh::language::source::dummyOrigin;

template<typename T>
T copy(const T &v) {
    return v;
}

TEST_CASE("Line location, construction, no parent") {
    LineLocation ll1(nullptr, dummyOrigin(), 0);
    LineLocation ll2 = ll1;
}

TEST_CASE("Line location, assignment") {
    LineLocation ll1 = dummyLineLocation();
    LineLocation ll2 = dummyLineLocation();
    ll1 = ll2;
}

TEST_CASE("Location, construction and assignment") {
    Location l1(dummyLineLocation(), 0);
    Location l2 = l1;
    l1 = l2;
}

TEST_CASE("Line location, construction, with parent") {
    std::shared_ptr<const Location> parent =
            std::make_shared<Location>(dummyLocation());
    LineLocation ll1(copy(parent), dummyOrigin(), 0);
    (void) ll1;
}

TEST_CASE("Line location, parent") {
    LineLocation ll1(nullptr, dummyOrigin(), 0);
    CHECK(ll1.parent() == nullptr);

    std::shared_ptr<const Location> parent =
            std::make_shared<Location>(dummyLocation());
    LineLocation ll2(copy(parent), dummyOrigin(), 0);
    CHECK(ll2.parent() == parent.get());
}

TEST_CASE("Line location, origin") {
    std::shared_ptr<const Origin> origin = dummyOrigin();
    LineLocation ll1(nullptr, copy(origin), 0);
    CHECK(&ll1.origin() == origin.get());
}

TEST_CASE("Line location, line") {
    LineLocation ll1(nullptr, dummyOrigin(), 0);
    LineLocation ll2(nullptr, dummyOrigin(), 1);
    LineLocation ll3(nullptr, dummyOrigin(), 2);
    CHECK(ll1.line() == 0);
    CHECK(ll2.line() == 1);
    CHECK(ll3.line() == 2);
}

TEST_CASE("Line location, comparison, no parent") {
    std::shared_ptr<const Location> parent =
            std::make_shared<Location>(dummyLocation());
    std::shared_ptr<const Origin> origin = dummyOrigin();

    LineLocation ll1(nullptr, copy(origin), 0);
    LineLocation ll2(copy(parent), copy(origin), 0);
    LineLocation ll3(nullptr, dummyOrigin(), 0);
    LineLocation ll4(nullptr, copy(origin), 1);

    CHECK(ll1 == ll1);
    CHECK(ll1 == LineLocation(ll1));
    CHECK(ll2 == ll2);
    CHECK(ll3 == ll3);
    CHECK(ll4 == ll4);
    CHECK_FALSE(ll1 != ll1);
    CHECK_FALSE(ll2 != ll2);
    CHECK_FALSE(ll3 != ll3);
    CHECK_FALSE(ll4 != ll4);

    CHECK_FALSE(ll1 == ll2);
    CHECK_FALSE(ll1 == ll3);
    CHECK_FALSE(ll1 == ll4);
    CHECK(ll1 != ll2);
    CHECK(ll1 != ll3);
    CHECK(ll1 != ll4);
}

TEST_CASE("Location, column") {
    Location l1(dummyLineLocation(), 0);
    Location l2(dummyLineLocation(), 1);
    Location l3(dummyLineLocation(), 2);
    CHECK(l1.column() == 0);
    CHECK(l2.column() == 1);
    CHECK(l3.column() == 2);
}

TEST_CASE("Location, comparison") {
    Location l1(dummyLineLocation(0), 0);
    Location l2(dummyLineLocation(1), 0);
    Location l3(dummyLineLocation(0), 1);

    CHECK(l1 == l1);
    CHECK(l1 == Location(l1));
    CHECK(l2 == l2);
    CHECK(l3 == l3);
    CHECK_FALSE(l1 != l1);
    CHECK_FALSE(l2 != l2);
    CHECK_FALSE(l3 != l3);

    CHECK_FALSE(l1 == l2);
    CHECK_FALSE(l1 == l3);
    CHECK(l1 != l2);
    CHECK(l1 != l3);
}

TEST_CASE("Line location, comparison, with parent") {
    std::shared_ptr<const Origin> origin = dummyOrigin();

    LineLocation ll1(nullptr, copy(origin), 0);
    std::shared_ptr<const Location> parent1 =
            std::make_shared<Location>(ll1, 0);
    LineLocation ll2(copy(parent1), copy(origin), 0);

    CHECK(ll2 == ll2);
    CHECK(ll2 == LineLocation(ll2));
    CHECK_FALSE(ll2 != ll2);

    std::shared_ptr<const Location> parent2 =
            std::make_shared<Location>(ll2, 0);
    std::shared_ptr<const Location> parent3 =
            std::make_shared<Location>(ll2, 0);
    LineLocation ll3(copy(parent2), copy(origin), 0);
    LineLocation ll4(copy(parent3), copy(origin), 0);
    LineLocation ll5(copy(parent3), copy(origin), 0);

    CHECK(ll2 != ll3);
    CHECK_FALSE(ll2 == ll3);

    CHECK(ll3 == ll4);
    CHECK(ll4 == ll5);
    CHECK_FALSE(ll3 != ll4);
    CHECK_FALSE(ll4 != ll5);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
