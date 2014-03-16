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
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/LineLocation.hh"
#include "language/source/LineLocationTestHelper.hh"

namespace {

using sesh::common::String;
using sesh::language::source::LineLocation;
using sesh::language::source::dummyLineLocation;

TEST_CASE("Line location, construction") {
    std::shared_ptr<const String> name =
            std::make_shared<String>(L("test*name"));

    LineLocation ll1(name, 0);
    LineLocation ll2(decltype(name)(name), 1);

    name = std::make_shared<String>(L("another name"));

    LineLocation ll3(name, 123);
    LineLocation ll4(decltype(name)(name), 987);

    CHECK(ll1.name() == L("test*name"));
    CHECK(ll1.line() == 0);
    CHECK(ll2.name() == L("test*name"));
    CHECK(ll2.line() == 1);
    CHECK(ll3.name() == L("another name"));
    CHECK(ll3.line() == 123);
    CHECK(ll4.name() == L("another name"));
    CHECK(ll4.line() == 987);

    ll1 = ll3;
    ll2 = std::move(ll4);

    CHECK(ll1.name() == L("another name"));
    CHECK(ll1.line() == 123);
    CHECK(ll2.name() == L("another name"));
    CHECK(ll2.line() == 987);
}

TEST_CASE("Line location, constructor null pointer exception") {
    std::shared_ptr<const String> name(nullptr);
    CHECK_THROWS_AS(LineLocation(name, 0), std::invalid_argument);
    CHECK_THROWS_AS(LineLocation(std::move(name), 0), std::invalid_argument);
}

TEST_CASE("Line location, comparison") {
    LineLocation ll1 = dummyLineLocation(L("apple"), 0);
    LineLocation ll2 = dummyLineLocation(L("apple"), 0);
    LineLocation ll3 = dummyLineLocation(L("banana"), 0);
    LineLocation ll4 = dummyLineLocation(L("apple"), 1);

    CHECK(ll1 == ll1);
    CHECK_FALSE(!(ll1 == ll1));
    CHECK(ll1 == ll2);
    CHECK_FALSE(!(ll1 == ll2));
    CHECK_FALSE(ll1 == ll3);
    CHECK(!(ll1 == ll3));
    CHECK_FALSE(ll1 == ll4);
    CHECK(!(ll1 == ll4));
    CHECK_FALSE(ll3 == ll4);
    CHECK(!(ll3 == ll4));
}

TEST_CASE("Dummy source line location") {
    LineLocation ll1 = dummyLineLocation();
    LineLocation ll2 = dummyLineLocation(L("test"));
    LineLocation ll3 = dummyLineLocation(L("test"), 777);

    CHECK(ll1.name() == L("dummy"));
    CHECK(ll1.line() == 0);
    CHECK(ll2.name() == L("test"));
    CHECK(ll2.line() == 0);
    CHECK(ll3.name() == L("test"));
    CHECK(ll3.line() == 777);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
