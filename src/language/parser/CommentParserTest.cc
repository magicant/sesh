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
#include "language/parser/CommentParser.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"

namespace {

using sesh::language::parser::CommentParser;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::SourceTestEnvironment;

class CommentParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Comment parser, construction") {
    CommentParserTestEnvironment e;
    CommentParser p(e);
    CommentParser(e, LineContinuationTreatment::LITERAL);
    CommentParser(std::move(p));
}

TEST_CASE("Comment parser, success with removed line continuation") {
    CommentParserTestEnvironment e;
    e.appendSource(L("\\\n#"));
    e.setIsEof();

    CommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#"));
    CHECK(e.position() == e.length());
}

TEST_CASE("Comment parser, failure with removed line continuation") {
    CommentParserTestEnvironment e;
    e.appendSource(L(" "));

    CommentParser p(e);
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Comment parser, success with literal line continuation") {
    CommentParserTestEnvironment e;
    e.appendSource(L("#"));
    e.setIsEof();

    CommentParser p(e, LineContinuationTreatment::LITERAL);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#"));
    CHECK(e.position() == e.length());
}

TEST_CASE("Comment parser, failure with literal line continuation") {
    CommentParserTestEnvironment e;
    e.appendSource(L("\\\n#"));

    CommentParser p(e, LineContinuationTreatment::LITERAL);
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Comment parser, stop at newline") {
    CommentParserTestEnvironment e;
    e.appendSource(L("# foo # bar\\\n"));

    CommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("# foo # bar\\"));
    CHECK(e.position() == e.length() - 1);
}

TEST_CASE("Comment parser, reset") {
    CommentParserTestEnvironment e;
    e.appendSource(L("#1\n#2\n"));

    CommentParser p(e);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#1"));
    CHECK(e.position() == 2);

    p.reset();
    e.setPosition(3);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == L("#2"));
    CHECK(e.position() == 5);

    p.reset();
    CHECK_FALSE(p.parse().hasValue());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
