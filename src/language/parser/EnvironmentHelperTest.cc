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

#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/EnvironmentHelper.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"

namespace {

using sesh::common::CharTraits;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::charIntAt;

class CharIntTestEnvironment :
        public SourceTestEnvironment, public EofEnvironment {
};

TEST_CASE("Char int at, empty, not eof") {
    CharIntTestEnvironment e;
    CHECK_THROWS_AS(charIntAt(e, 0), IncompleteParse);
}

TEST_CASE("Char int at, empty, eof") {
    CharIntTestEnvironment e;
    e.setIsEof();
    CHECK(charIntAt(e, 0) == CharTraits::eof());
}

TEST_CASE("Char int at, middle, not eof") {
    CharIntTestEnvironment e;
    e.setSource(L("ABC"));
    CHECK(charIntAt(e, 1) == CharTraits::to_int_type(L('B')));
}

TEST_CASE("Char int at, middle, eof") {
    CharIntTestEnvironment e;
    e.setSource(L("ABC"));
    e.setIsEof();
    CHECK(charIntAt(e, 1) == CharTraits::to_int_type(L('B')));
}

TEST_CASE("Char int at, end, not eof") {
    CharIntTestEnvironment e;
    e.setSource(L("Hello"));
    CHECK_THROWS_AS(charIntAt(e, 5), IncompleteParse);
}

TEST_CASE("Char int at, end, eof") {
    CharIntTestEnvironment e;
    e.setSource(L("Hello"));
    e.setIsEof();
    CHECK(charIntAt(e, 5) == CharTraits::eof());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
