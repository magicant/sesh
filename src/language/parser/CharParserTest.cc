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
#include "common/Maybe.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"

namespace {

using sesh::common::Char;
using sesh::common::Maybe;
using sesh::language::parser::CharParser;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::SourceTestEnvironment;

template<Char expected>
bool is(const Environment &, Char c) { return c == expected; }

template<Char expected, bool result>
bool expect(const Environment &, Char c) {
    CHECK(c == expected);
    return result;
}

bool fail(const Environment &, Char) {
    throw "unexpected call to predicate";
}

class CharParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Char parser, construction and assignment") {
    CharParserTestEnvironment e;
    CharParser p1(e, is<L('\0')>);
    CharParser p2(p1);
    p1 = p2;
}

TEST_CASE("Char parser, success") {
    CharParserTestEnvironment e;
    CharParser p(e, expect<L('A'), true>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("A"));
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == L('A'));
    CHECK(e.position() == e.length());
}

TEST_CASE("Char parser, failure, false predicate") {
    CharParserTestEnvironment e;
    CharParser p(e, expect<L('B'), false>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("B"));
    CHECK(p.parse() == nullptr);
    CHECK(e.position() == 0);
}

TEST_CASE("Char parser, failure, eof") {
    CharParserTestEnvironment e;
    CharParser p(e, fail);

    e.setIsEof();
    CHECK(p.parse() == nullptr);
    CHECK(e.position() == 0);
}

TEST_CASE("Char parser, remove line continuations") {
    CharParserTestEnvironment e;
    CharParser p(e, expect<L('C'), true>, LineContinuationTreatment::REMOVE);

    e.appendSource(L("\\\n\\\n"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.checkSource(L(""));
    e.appendSource(L("\\\n\\\nC"));
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == L('C'));
    CHECK(e.position() == e.length());
    e.checkSource(L("C"));
}

TEST_CASE("Char parser, keep line continuations") {
    CharParserTestEnvironment e;
    CharParser p(e, expect<L('\\'), true>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("\\\n"));
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == L('\\'));
    CHECK(e.position() == 1);
}

TEST_CASE("Char parser, reset") {
    CharParserTestEnvironment e;
    CharParser p(e, is<L('A')>);
    e.appendSource(L("A"));
    CHECK(p.parse() != nullptr);

    p.reset();
    e.appendSource(L("B"));
    CHECK(p.parse() == nullptr);

    p.reset(is<L('B')>);
    CHECK(p.parse() != nullptr);

    p.reset(is<L('\\')>, LineContinuationTreatment::LITERAL);
    e.appendSource(L("\\\nC"));
    CHECK(p.parse() != nullptr);
    CHECK(e.position() == 3);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
