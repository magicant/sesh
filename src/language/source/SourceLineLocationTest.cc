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
#include <utility>
#include "language/source/SourceLineLocation.hh"
#include "language/source/SourceLineLocationTestHelper.hh"

using sesh::language::source::SourceLineLocation;
using sesh::language::source::dummySourceLineLocation;

TEST_CASE("Source line location construction") {
    std::shared_ptr<const std::wstring> name =
            std::make_shared<std::wstring>(L"test*name");

    SourceLineLocation sll1(name, 0);
    SourceLineLocation sll2(decltype(name)(name), 1);

    name = std::make_shared<std::wstring>(L"another name");

    SourceLineLocation sll3(name, 123);
    SourceLineLocation sll4(decltype(name)(name), 987);

    CHECK(sll1.name() == L"test*name");
    CHECK(sll1.line() == 0);
    CHECK(sll2.name() == L"test*name");
    CHECK(sll2.line() == 1);
    CHECK(sll3.name() == L"another name");
    CHECK(sll3.line() == 123);
    CHECK(sll4.name() == L"another name");
    CHECK(sll4.line() == 987);

    sll1 = sll3;
    sll2 = std::move(sll4);

    CHECK(sll1.name() == L"another name");
    CHECK(sll1.line() == 123);
    CHECK(sll2.name() == L"another name");
    CHECK(sll2.line() == 987);
}

TEST_CASE("Source line location constructor null pointer exception") {
    std::shared_ptr<const std::wstring> name(nullptr);
    CHECK_THROWS_AS(
            SourceLineLocation(name, 0),
            std::invalid_argument);
    CHECK_THROWS_AS(
            SourceLineLocation(std::move(name), 0),
            std::invalid_argument);
}

TEST_CASE("Source line location comparison") {
    SourceLineLocation sll1 = dummySourceLineLocation(L"apple", 0);
    SourceLineLocation sll2 = dummySourceLineLocation(L"apple", 0);
    SourceLineLocation sll3 = dummySourceLineLocation(L"banana", 0);
    SourceLineLocation sll4 = dummySourceLineLocation(L"apple", 1);

    CHECK(sll1 == sll1);
    CHECK_FALSE(!(sll1 == sll1));
    CHECK(sll1 == sll2);
    CHECK_FALSE(!(sll1 == sll2));
    CHECK_FALSE(sll1 == sll3);
    CHECK(!(sll1 == sll3));
    CHECK_FALSE(sll1 == sll4);
    CHECK(!(sll1 == sll4));
    CHECK_FALSE(sll3 == sll4);
    CHECK(!(sll3 == sll4));
}

TEST_CASE("Dummy source line location") {
    SourceLineLocation sll1 = dummySourceLineLocation();
    SourceLineLocation sll2 = dummySourceLineLocation(L"test");
    SourceLineLocation sll3 = dummySourceLineLocation(L"test", 777);

    CHECK(sll1.name() == L"dummy");
    CHECK(sll1.line() == 0);
    CHECK(sll2.name() == L"test");
    CHECK(sll2.line() == 0);
    CHECK(sll3.name() == L"test");
    CHECK(sll3.line() == 777);
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
