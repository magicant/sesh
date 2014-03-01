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

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/RawStringParser.hh"
#include "language/syntax/RawString.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::RawStringParser;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::syntax::RawString;

class RawStringParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

template<Char expected>
bool isNot(const Environment &, Char c) {
    return c != expected;
}

TEST_CASE("Raw string parser, construction") {
    RawStringParserTestEnvironment e;
    RawStringParser p1(e, isNot<L('\0')>);
    RawStringParser p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Raw string parser, success") {
    RawStringParserTestEnvironment e;
    RawStringParser p(e, isNot<L('$')>);

    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(String(L("AB\0C$XYZ"), 8));

    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    RawString *rs = dynamic_cast<RawString *>(p.parse().value().get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == String(L("AB\0C"), 4));
    CHECK(e.position() == 4);
}

TEST_CASE("Raw string parser, empty result") {
    RawStringParserTestEnvironment e;
    e.appendSource(L("X"));
    e.setPosition(1);

    RawStringParser p(e, isNot<L('*')>);

    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("***"));

    CHECK_FALSE(p.parse());
    CHECK(e.position() == 1);
}

TEST_CASE("Raw string parser, reset") {
    RawStringParserTestEnvironment e;
    RawStringParser p(e, isNot<L('-')>);
    RawString *rs;

    e.appendSource(L("A-B-"));

    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    rs = dynamic_cast<RawString *>(p.parse().value().get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == L("A"));
    CHECK(e.position() == 1);

    p.reset();
    e.setPosition(2);

    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    rs = dynamic_cast<RawString *>(p.parse().value().get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == L("B"));
    CHECK(e.position() == 3);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
