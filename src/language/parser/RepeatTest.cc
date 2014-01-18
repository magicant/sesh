/* Copyright (C) 2013-2014 WATANABE Yuki
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
#include "language/parser/CharParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Repeat.hh"

namespace {

using sesh::common::Char;
using sesh::language::parser::CharParser;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::Repeat;
using sesh::language::parser::SourceTestEnvironment;

class RepeatTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

Repeat<CharParser> charRepeat(Environment &e, Char reject) {
    return Repeat<CharParser>::create(
            e,
            [reject](const Environment &, Char c) { return c != reject; },
            LineContinuationTreatment::LITERAL);
}

TEST_CASE("Repeat, empty results") {
    RepeatTestEnvironment e;
    Repeat<CharParser> r = charRepeat(e, L('A'));

    CHECK_THROWS_AS(r.parse(), IncompleteParse);
    e.appendSource(L("A"));
    REQUIRE(r.parse().hasValue());
    CHECK(r.parse().value().empty());
}

TEST_CASE("Repeat, three results") {
    RepeatTestEnvironment e;
    Repeat<CharParser> r = charRepeat(e, L('X'));

    CHECK_THROWS_AS(r.parse(), IncompleteParse);
    e.appendSource(L("ABCX"));
    REQUIRE(r.parse().hasValue());
    CHECK(e.position() == 3);
    CHECK(r.parse().value().size() == 3);
    CHECK(r.parse().value().at(0) == L('A'));
    CHECK(r.parse().value().at(1) == L('B'));
    CHECK(r.parse().value().at(2) == L('C'));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
