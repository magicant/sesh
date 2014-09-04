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
#include "language/source/location.hh"
#include "language/source/location_test_helper.hh"
#include "language/source/OriginTestHelper.hh"

namespace {

using sesh::language::source::Origin;
using sesh::language::source::dummyLineLocation;
using sesh::language::source::dummyLocation;
using sesh::language::source::dummyOrigin;
using sesh::language::source::line_location;
using sesh::language::source::location;

template<typename T>
T copy(const T &v) {
    return v;
}

TEST_CASE("Line location, construction, no parent") {
    line_location ll1(nullptr, dummyOrigin(), 0);
    line_location ll2 = ll1;
}

TEST_CASE("Line location, assignment") {
    line_location ll1 = dummyLineLocation();
    line_location ll2 = dummyLineLocation();
    ll1 = ll2;
}

TEST_CASE("Location, construction and assignment") {
    location l1(dummyLineLocation(), 0);
    location l2 = l1;
    l1 = l2;
}

TEST_CASE("Line location, construction, with parent") {
    std::shared_ptr<const location> parent =
            std::make_shared<location>(dummyLocation());
    line_location ll1(copy(parent), dummyOrigin(), 0);
    (void) ll1;
}

TEST_CASE("Line location, parent") {
    line_location ll1(nullptr, dummyOrigin(), 0);
    CHECK(ll1.parent() == nullptr);

    std::shared_ptr<const location> parent =
            std::make_shared<location>(dummyLocation());
    line_location ll2(copy(parent), dummyOrigin(), 0);
    CHECK(ll2.parent() == parent.get());
}

TEST_CASE("Line location, origin") {
    std::shared_ptr<const Origin> origin = dummyOrigin();
    line_location ll1(nullptr, copy(origin), 0);
    CHECK(&ll1.origin() == origin.get());
}

TEST_CASE("Line location, line") {
    line_location ll1(nullptr, dummyOrigin(), 0);
    line_location ll2(nullptr, dummyOrigin(), 1);
    line_location ll3(nullptr, dummyOrigin(), 2);
    CHECK(ll1.line() == 0);
    CHECK(ll2.line() == 1);
    CHECK(ll3.line() == 2);
}

TEST_CASE("Line location, comparison, no parent") {
    std::shared_ptr<const location> parent =
            std::make_shared<location>(dummyLocation());
    std::shared_ptr<const Origin> origin = dummyOrigin();

    line_location ll1(nullptr, copy(origin), 0);
    line_location ll2(copy(parent), copy(origin), 0);
    line_location ll3(nullptr, dummyOrigin(), 0);
    line_location ll4(nullptr, copy(origin), 1);

    CHECK(ll1 == ll1);
    CHECK(ll1 == line_location(ll1));
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
    location l1(dummyLineLocation(), 0);
    location l2(dummyLineLocation(), 1);
    location l3(dummyLineLocation(), 2);
    CHECK(l1.column() == 0);
    CHECK(l2.column() == 1);
    CHECK(l3.column() == 2);
}

TEST_CASE("Location, comparison") {
    location l1(dummyLineLocation(0), 0);
    location l2(dummyLineLocation(1), 0);
    location l3(dummyLineLocation(0), 1);

    CHECK(l1 == l1);
    CHECK(l1 == location(l1));
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

    line_location ll1(nullptr, copy(origin), 0);
    std::shared_ptr<const location> parent1 =
            std::make_shared<location>(ll1, 0);
    line_location ll2(copy(parent1), copy(origin), 0);

    CHECK(ll2 == ll2);
    CHECK(ll2 == line_location(ll2));
    CHECK_FALSE(ll2 != ll2);

    std::shared_ptr<const location> parent2 =
            std::make_shared<location>(ll2, 0);
    std::shared_ptr<const location> parent3 =
            std::make_shared<location>(ll2, 0);
    line_location ll3(copy(parent2), copy(origin), 0);
    line_location ll4(copy(parent3), copy(origin), 0);
    line_location ll5(copy(parent3), copy(origin), 0);

    CHECK(ll2 != ll3);
    CHECK_FALSE(ll2 == ll3);

    CHECK(ll3 == ll4);
    CHECK(ll4 == ll5);
    CHECK_FALSE(ll3 != ll4);
    CHECK_FALSE(ll4 != ll5);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
