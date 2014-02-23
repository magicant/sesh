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
#include "language/parser/BlankAndCommentParser.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/LineContinuationEnvironment.hh"

namespace {

using sesh::language::parser::BlankAndCommentParser;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SourceTestEnvironment;

class BlankAndCommentParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

TEST_CASE("Blank and comment parser, construction") {
    BlankAndCommentParserTestEnvironment e;
    BlankAndCommentParser p(e);
    BlankAndCommentParser(std::move(p));
}

TEST_CASE("Blank and comment parser, space and tab and line continuation") {
    BlankAndCommentParserTestEnvironment e;
    e.appendSource(L(" \\\n\t\n"));

    BlankAndCommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L(" \t"));
    CHECK(e.position() == e.length() - 1);
}

TEST_CASE("Blank and comment parser, no blank or comment") {
    BlankAndCommentParserTestEnvironment e;
    e.appendSource(L("X"));

    BlankAndCommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L(""));
    CHECK(e.position() == e.length() - 1);
}

TEST_CASE("Blank and comment parser, comment only") {
    BlankAndCommentParserTestEnvironment e;
    e.appendSource(L("#comment\n"));

    BlankAndCommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#comment"));
    CHECK(e.position() == e.length() - 1);
}

TEST_CASE("Blank and comment parser, blank and comment") {
    BlankAndCommentParserTestEnvironment e;
    e.appendSource(L("\t #\\\n"));

    BlankAndCommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("\t #\\"));
    CHECK(e.position() == e.length() - 1);
}

TEST_CASE("Blank and comment parser, reset") {
    BlankAndCommentParserTestEnvironment e;
    e.appendSource(L("\t#\\\n ##\n"));

    BlankAndCommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("\t#\\"));
    CHECK(e.position() == 3);

    p.reset();
    e.setPosition(4);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L(" ##"));
    CHECK(e.position() == 7);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
