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

#include <utility>
#include "common/Char.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NewlineParser.hh"

namespace {

using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NewlineParser;
using sesh::language::parser::SourceTestEnvironment;

class NewlineParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Newline parser, construction") {
    NewlineParserTestEnvironment e;
    NewlineParser p1(e);
    NewlineParser p2(e, LineContinuationTreatment::LITERAL);
    NewlineParser p3(std::move(p2));
    p2 = std::move(p3);
}

TEST_CASE("Newline parser, no newline") {
    NewlineParserTestEnvironment e;
    NewlineParser p(e);
    e.appendSource(L("\\\n\\\nX"));
    CHECK_FALSE(p.parse().hasValue());
    e.checkSource(L("X"));
}

TEST_CASE("Newline parser, literal line continuation") {
    NewlineParserTestEnvironment e;
    NewlineParser p(e, LineContinuationTreatment::LITERAL);
    e.appendSource(L("\\\n\\\n\n"));
    CHECK_FALSE(p.parse().hasValue());
    e.checkSource(L("\\\n\\\n\n"));
}

TEST_CASE("Newline parser, newline without here-document") {
    NewlineParserTestEnvironment e;
    NewlineParser p(e);
    e.appendSource(L("\\\n\n\\\n"));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L('\n'));
    e.checkSource(L("\n\\\n"));
    CHECK(e.position() == 1);
}

TEST_CASE("Newline parser, reset") {
    NewlineParserTestEnvironment e;
    NewlineParser p(e);
    e.appendSource(L("\\\n\n\\\n\n"));

    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L('\n'));
    e.checkSource(L("\n\\\n\n"));
    CHECK(e.position() == 1);

    p.reset();

    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L('\n'));
    e.checkSource(L("\n\n"));
    CHECK(e.position() == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
