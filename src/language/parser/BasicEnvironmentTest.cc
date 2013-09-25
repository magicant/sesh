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

#include <stdexcept>
#include <utility>
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/NeedMoreSource.hh"

namespace {

using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::NeedMoreSource;

TEST_CASE("Environment remove line continuation 1") {
    BasicEnvironmentStub e;

    INFO("1 empty source");
    e.checkSource(L(""));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin()), NeedMoreSource);
    INFO("2 empty source");
    e.checkSource(L(""));
}

TEST_CASE("Environment remove line continuation 2") {
    BasicEnvironmentStub e;

    e.setSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin()), NeedMoreSource);
    INFO("1 \\\\");
    e.checkSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 1), NeedMoreSource);
    INFO("2 \\\\");
    e.checkSource(L("\\"));
}

TEST_CASE("Environment remove line continuation 3") {
    BasicEnvironmentStub e;

    e.setSource(L("\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin()));
    INFO("1 \\n");
    e.checkSource(L("\n"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 1), NeedMoreSource);
    INFO("2 \\n");
    e.checkSource(L("\n"));
}

TEST_CASE("Environment remove line continuation 4") {
    BasicEnvironmentStub e;

    e.setSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin()));
    INFO("1 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 3));
    INFO("2 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 4));
    INFO("3 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 5));
    INFO("4 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 7));
    INFO("5 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 8));
    INFO("6 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 9), NeedMoreSource);
    INFO("7 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK(e.removeLineContinuation(e.begin() + 6));
    INFO("Test\\n\\\\\\n");
    e.checkSource(L("Test\n\\\n"));

    CHECK(e.removeLineContinuation(e.begin() + 5));
    INFO("Test\\n");
    e.checkSource(L("Test\n"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
