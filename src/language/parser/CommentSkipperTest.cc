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

#include "common/Char.hh"
#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Skipper.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::common::Char;
using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::CommentSkipper;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Skipper;
using sesh::language::source::Source;
using sesh::language::source::SourceStub;

bool isExclamationOrHash(const Environment &, Char c) {
    return c == L('!') || c == L('#');
}

bool isDollarOrHashOrNewline(const Environment &, Char c) {
    return c == L('$') || c == L('#') || c == L('\n');
}

TEST_CASE("Comment skipper construction") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));
    CommentSkipper cs2(cs);
    CommentSkipper(std::move(cs2));
}

TEST_CASE("Comment skipper, no append, no skip") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    e.appendSource(L("!"));

    cs.skip();
    CHECK(e.current() == e.begin());
    cs.skip();
    CHECK(e.current() == e.begin());

    e.current() += 1;
    REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);

    e.setIsEof();
    cs.skip();
    CHECK(e.current() == e.begin() + 1);
    cs.skip();
    CHECK(e.current() == e.begin() + 1);
}

TEST_CASE("Comment skipper, no append, skip blanks") {
    BasicEnvironmentStub e;
    e.appendSource(L(" $ $!\n!"));

    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    cs.skip();
    CHECK(e.current() == e.begin() + 4);
    cs.skip();
    CHECK(e.current() == e.begin() + 4);

    e.current() += 1;
    cs.skip();
    CHECK(e.current() == e.begin() + 6);
    cs.skip();
    CHECK(e.current() == e.begin() + 6);
}

TEST_CASE("Comment skipper, 1 append, skip blanks") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);

    e.appendSource(L("!--!"));
    cs.skip();
    CHECK(e.current() == e.begin());
    cs.skip();
    CHECK(e.current() == e.begin());

    e.current() += 1;
    cs.skip();
    CHECK(e.current() == e.begin() + 3);
    cs.skip();
    CHECK(e.current() == e.begin() + 3);
}

TEST_CASE("Comment skipper, no append, skip comments") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isDollarOrHashOrNewline));

    e.appendSource(L("# $\\\n##\n"));
    cs.skip();
    CHECK(e.current() == e.begin() + 4);

    e.current() += 1;
    cs.skip();
    CHECK(e.current() == e.begin() + 7);
}

TEST_CASE("Comment skipper, 1 append, skip comments") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isDollarOrHashOrNewline));

    e.appendSource(L("#"));
    REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);

    e.appendSource(L("$\n"));
    cs.skip();
    CHECK(e.current() == e.begin() + 2);
}

TEST_CASE("Comment skipper, no append, skip blanks and comments") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    e.appendSource(L(" #\n#\n #!\n !"));
    cs.skip();
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("Comment skipper, many appends, skip blanks and comments") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    for (unsigned i = 0; i < 4; ++i) {
        e.appendSource(L(" "));
        REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);
        e.appendSource(L("#"));
        REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);
        e.appendSource(L("!"));
        REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);
        e.appendSource(L("\n"));
        REQUIRE_THROWS_AS(cs.skip(), NeedMoreSource);
    }

    e.setIsEof();
    cs.skip();
    CHECK(e.current() == e.end());
}

TEST_CASE("Comment skipper, unfinished comment") {
    BasicEnvironmentStub e;
    CommentSkipper cs(e, Skipper(e, isExclamationOrHash));

    e.appendSource(L("#"));
    e.setIsEof();
    cs.skip();
    CHECK(e.current() == e.end());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
