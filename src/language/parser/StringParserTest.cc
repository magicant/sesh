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

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/StringParser.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::StringParser;

template<Char c>
bool is(const Environment &, Char c2) {
    return c == c2;
}

TEST_CASE("String parser construction") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('\0')>);
    StringParser p2(p);
    StringParser(std::move(p2));
}

TEST_CASE("String parser, 0 append") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('$')>);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(String(L("AB\0C$XYZ"), 8));

    String result = p.parse();
    CHECK(result == String(L("AB\0C"), 4));
    CHECK(e.current() == e.begin() + 4);
}

TEST_CASE("String parser, 1 append") {
    BasicEnvironmentStub e;
    e.appendSource(L("ABC+XYZ"));
    e.current() += 4;

    StringParser p(e, is<L('-')>);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("$123-+"));

    String result = p.parse();
    CHECK(result == L("XYZ$123"));
    CHECK(e.current() == e.end() - 2);
}

TEST_CASE("String parser, 0 append, to end") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('x')>);
    e.appendSource(L("apple"));
    e.setIsEof();

    String result = p.parse();
    CHECK(result == L("apple"));
    CHECK(e.current() == e.end());
}

TEST_CASE("String parser, 1 append, to end") {
    BasicEnvironmentStub e;
    e.appendSource(L("hot do"));
    e.current() += 4;

    StringParser p(e, is<L('x')>);
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("g"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.setIsEof();

    String result = p.parse();
    CHECK(result == L("dog"));
    CHECK(e.current() == e.end());
}

TEST_CASE("String parser, 1 append, empty result") {
    BasicEnvironmentStub e;
    e.appendSource(L("X"));
    e.current() += 1;

    StringParser p(e, is<L('*')>);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("***"));

    String result = p.parse();
    CHECK(result == L(""));
    CHECK(e.current() == e.end() - 3);
}

TEST_CASE("String parser, null delimiter") {
    BasicEnvironmentStub e;
    StringParser p(e, nullptr);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("Hello"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L(", "));
    e.appendSource(L("world"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("!"));
    e.setIsEof();

    String result = p.parse();
    CHECK(result == L("Hello, world!"));
    CHECK(e.current() == e.end());
}

TEST_CASE("String parser, no remove line continuations 1") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('@')>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("AB\\\nC@"));

    String result = p.parse();
    CHECK(result == L("AB\\\nC"));
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("String parser, no remove line continuations 2") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('\\')>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("AB\\\nC@"));

    String result = p.parse();
    CHECK(result == L("AB"));
    CHECK(e.current() == e.begin() + 2);
}

TEST_CASE("String parser, remove line continuations 1") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('@')>, LineContinuationTreatment::REMOVE);

    e.appendSource(L("ABC\\\\\n\nDEF\\"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\nGHI@"));

    String result = p.parse();
    CHECK(result == L("ABC\\\nDEFGHI"));
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("String parser remove line continuations 2") {
    BasicEnvironmentStub e;
    StringParser p(e, is<L('\\')>, LineContinuationTreatment::REMOVE);

    e.appendSource(L("ABC\\\nDEF\\"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.setIsEof();

    String result = p.parse();
    CHECK(result == L("ABCDEF"));
    CHECK(e.current() == e.end() - 1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
