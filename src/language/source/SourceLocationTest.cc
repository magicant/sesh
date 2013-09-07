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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <memory>
#include <stdexcept>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/SourceLocation.hh"
#include "language/source/SourceLocationTestHelper.hh"

namespace {

using sesh::common::String;
using sesh::language::source::SourceLocation;
using sesh::language::source::dummySourceLocation;

TEST_CASE("Source location construction") {
    std::shared_ptr<const String> name = std::make_shared<String>(L("12"));

    SourceLocation sl1(name, 0, 0);
    SourceLocation sl2(decltype(name)(name), 1, 1);

    name = std::make_shared<String>(L("34"));

    SourceLocation sl3(name, 3, 4);
    SourceLocation sl4(decltype(name)(name), 1234, 9876);

    CHECK(sl1.name() == L("12"));
    CHECK(sl1.line() == 0);
    CHECK(sl1.column() == 0);
    CHECK(sl2.name() == L("12"));
    CHECK(sl2.line() == 1);
    CHECK(sl2.column() == 1);
    CHECK(sl3.name() == L("34"));
    CHECK(sl3.line() == 3);
    CHECK(sl3.column() == 4);
    CHECK(sl4.name() == L("34"));
    CHECK(sl4.line() == 1234);
    CHECK(sl4.column() == 9876);

    sl1 = sl2;
    CHECK(sl1.line() == 1);
    sl1 = std::move(sl3);
    CHECK(sl1.line() == 3);
}

TEST_CASE("Source location constructor null pointer exception") {
    std::shared_ptr<const String> name(nullptr);
    CHECK_THROWS_AS(
            SourceLocation(name, 0, 0),
            std::invalid_argument);
    CHECK_THROWS_AS(
            SourceLocation(std::move(name), 0, 0),
            std::invalid_argument);
}

TEST_CASE("Source location comparison") {
    SourceLocation sl1 = dummySourceLocation(L("apple"), 0, 0);
    SourceLocation sl2 = dummySourceLocation(L("apple"), 0, 0);
    SourceLocation sl3 = dummySourceLocation(L("banana"), 0, 0);
    SourceLocation sl4 = dummySourceLocation(L("apple"), 1, 0);
    SourceLocation sl5 = dummySourceLocation(L("apple"), 0, 1);

    CHECK(sl1 == sl1);
    CHECK_FALSE(!(sl1 == sl1));
    CHECK(sl1 == sl2);
    CHECK_FALSE(!(sl1 == sl2));
    CHECK_FALSE(sl1 == sl3);
    CHECK(!(sl1 == sl3));
    CHECK_FALSE(sl1 == sl4);
    CHECK(!(sl1 == sl4));
    CHECK_FALSE(sl1 == sl5);
    CHECK(!(sl1 == sl5));
    CHECK_FALSE(sl3 == sl4);
    CHECK(!(sl3 == sl4));
    CHECK_FALSE(sl3 == sl5);
    CHECK(!(sl3 == sl5));
}

TEST_CASE("Dummy source location") {
    SourceLocation sl1 = dummySourceLocation();
    SourceLocation sl2 = dummySourceLocation(L("foo"));
    SourceLocation sl3 = dummySourceLocation(L("foo"), 123);
    SourceLocation sl4 = dummySourceLocation(L("foo"), 123, 456);

    CHECK(sl1.name() == L("dummy"));
    CHECK(sl1.line() == 1);
    CHECK(sl1.column() == 1);
    CHECK(sl2.name() == L("foo"));
    CHECK(sl2.line() == 1);
    CHECK(sl2.column() == 1);
    CHECK(sl3.name() == L("foo"));
    CHECK(sl3.line() == 123);
    CHECK(sl3.column() == 1);
    CHECK(sl4.name() == L("foo"));
    CHECK(sl4.line() == 123);
    CHECK(sl4.column() == 456);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
