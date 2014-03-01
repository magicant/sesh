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
#include "language/parser/BlankCommentNewlineParser.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/LineContinuationEnvironment.hh"

namespace {

using sesh::language::parser::BlankCommentNewlineParser;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SourceTestEnvironment;

class BlankCommentNewlineParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

TEST_CASE("Blank-comment-newline parser, construction and assignment") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p1(e);
    BlankCommentNewlineParser p2(p1);
    BlankCommentNewlineParser p3(std::move(p2));
    p1 = p3;
    p1 = std::move(p3);
}

TEST_CASE("Blank-comment-newline parser, nothing") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L("X"));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value().empty());
    e.checkSource(L("X"));
    CHECK(e.position() == 0);
}

TEST_CASE("Blank-comment-newline parser, blank only") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L("\t\\\nX"));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("\t"));
    e.checkSource(L("\tX"));
    CHECK(e.position() == 1);
}

TEST_CASE("Blank-comment-newline parser, comment only") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L("#X"));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#X"));
    e.checkSource(L("#X"));
    CHECK(e.position() == 2);
}

TEST_CASE("Blank-comment-newline parser, newline only") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L("\\\n\nX"));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("\n"));
    e.checkSource(L("\nX"));
    CHECK(e.position() == 1);
}

TEST_CASE("Blank-comment-newline parser, all") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L(" \\\n\t#\\\n \n"));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L(" \t#\\\n"));
    e.checkSource(L(" \t#\\\n \n"));
    CHECK(e.position() == 5);
}

TEST_CASE("Blank-comment-newline parser, reset") {
    BlankCommentNewlineParserTestEnvironment e;
    BlankCommentNewlineParser p(e);
    e.appendSource(L(" \\\n\t#\\\n \n"));

    p.parse();
    CHECK(e.position() == 5);

    p.reset();

    REQUIRE(p.parse().hasValue());
    CHECK(p.begin() == 5);
    CHECK(p.parse().value() == L(" \n"));
    CHECK(e.position() == 7);

    e.checkSource(L(" \t#\\\n \n"));
}

// TODO here-document parsing

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
