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
#include "common/String.hh"
#include "language/parser/CharFilter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"

namespace {

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::Maybe;
using sesh::language::parser::CharFilter;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::SourceTestEnvironment;

using CharInt = CharTraits::int_type;

#define IL(x) (CharTraits::to_int_type(L(x)))

template<CharInt expected>
bool is(const Environment &, CharInt i) { return i == expected; }

template<CharInt expected, bool result>
bool expect(const Environment &, CharInt i) {
    CHECK(i == expected);
    return result;
}

class CharFilterTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Char filter, construction and assignment") {
    CharFilterTestEnvironment e;
    CharFilter p1(e, is<IL('\0')>);
    CharFilter p2(p1);
    p1 = p2;
}

TEST_CASE("Char filter, success for normal char") {
    CharFilterTestEnvironment e;
    CharFilter p(e, expect<IL('A'), true>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("A"));
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == IL('A'));
    CHECK(e.position() == 0);
}

TEST_CASE("Char filter, success for eof") {
    CharFilterTestEnvironment e;
    CharFilter p(e, expect<CharTraits::eof(), true>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.setIsEof();
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == CharTraits::eof());
    CHECK(e.position() == 0);
}

TEST_CASE("Char filter, failure for normal char") {
    CharFilterTestEnvironment e;
    CharFilter p(e, expect<IL('B'), false>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("B"));
    CHECK_FALSE(p.parse() != nullptr);
    CHECK(e.position() == 0);
}

TEST_CASE("Char filter, failure for eof") {
    CharFilterTestEnvironment e;
    CharFilter p(e, expect<CharTraits::eof(), false>);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.setIsEof();
    CHECK_FALSE(p.parse() != nullptr);
    CHECK(e.position() == 0);
}

TEST_CASE("Char filter, remove line continuations") {
    CharFilterTestEnvironment e;
    CharFilter p(
            e,
            expect<CharTraits::eof(), true>,
            LineContinuationTreatment::REMOVE);

    e.appendSource(L("\\\n\\\n"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.checkSource(L(""));
    e.appendSource(L("\\\n\\\n"));
    e.setIsEof();
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == CharTraits::eof());
    CHECK(e.position() == 0);
    e.checkSource(L(""));
}

TEST_CASE("Char filter, keep line continuations") {
    CharFilterTestEnvironment e;
    CharFilter p(e, expect<L('\\'), true>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("\\\n"));
    REQUIRE(p.parse() != nullptr);
    CHECK(*p.parse() == IL('\\'));
    CHECK(e.position() == 0);
}

TEST_CASE("Char filter, reset") {
    CharFilterTestEnvironment e;
    CharFilter p(e, is<IL('A')>);
    e.appendSource(L("A"));
    CHECK(p.parse() != nullptr);

    p.reset();
    e.setPosition(e.length());
    e.appendSource(L("\\\nB"));
    CHECK_FALSE(p.parse() != nullptr);

    p.reset(is<IL('B')>);
    CHECK(p.parse() != nullptr);

    p.reset(is<IL('\\')>, LineContinuationTreatment::LITERAL);
    e.setPosition(e.length());
    e.appendSource(L("\\\nC"));
    CHECK(p.parse() != nullptr);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
