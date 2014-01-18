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

#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"

namespace {

using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SourceTestEnvironment;

class LineContinuationTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Line continuation environment, remove, empty") {
    LineContinuationTestEnvironment e;

    CHECK_THROWS_AS(e.removeLineContinuation(0), IncompleteParse);
    e.checkSource(L(""));
}

TEST_CASE("Line continuation environment, remove, before backslash") {
    LineContinuationTestEnvironment e;
    e.setSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(0), IncompleteParse);
    e.checkSource(L("\\"));
}

TEST_CASE("Line continuation environment, remove, after backslash") {
    LineContinuationTestEnvironment e;
    e.setSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(1), IncompleteParse);
    e.checkSource(L("\\"));
}

TEST_CASE("Line continuation environment, remove, before newline") {
    LineContinuationTestEnvironment e;
    e.setSource(L("\n"));

    CHECK_FALSE(e.removeLineContinuation(0));
    e.checkSource(L("\n"));
}

TEST_CASE("Line continuation environment, remove, after newline") {
    LineContinuationTestEnvironment e;
    e.setSource(L("\n"));

    CHECK_THROWS_AS(e.removeLineContinuation(1), IncompleteParse);
    e.checkSource(L("\n"));
}

TEST_CASE("Line continuation environment, remove, success") {
    LineContinuationTestEnvironment e;
    e.setSource(L("A\\\\\n\nB"));

    CHECK(e.removeLineContinuation(2));
    e.checkSource(L("A\\\nB"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
