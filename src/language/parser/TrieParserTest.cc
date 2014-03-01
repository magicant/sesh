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

#include <memory>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "common/Trie.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/TrieParser.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::SourceTestEnvironment;

using Trie = sesh::common::Trie<Char, int>;
using TrieParser = sesh::language::parser::TrieParser<int>;

class TrieParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Trie parser, construction and assignment") {
    TrieParserTestEnvironment e;
    TrieParser p1(e, std::make_shared<const Trie>());
    TrieParser p2(p1);
    p1 = p2;
}

TEST_CASE("Trie parser, success") {
    TrieParserTestEnvironment e;
    e.appendSource(L("ABCD"));

    auto trie = std::make_shared<Trie>();
    trie->emplaceDescendants(String(L("AB"))).emplaceValue(1);
    trie->emplaceDescendants(String(L("ABCX"))).emplaceValue(2);
    trie->emplaceDescendants(String(L("X"))).emplaceValue(3);

    TrieParser p(e, std::move(trie));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == 1);
    CHECK(e.position() == 2);
}

TEST_CASE("Trie parser, failure") {
    TrieParserTestEnvironment e;
    e.appendSource(L("ABC"));

    auto trie = std::make_shared<Trie>();
    trie->emplaceDescendants(String(L("AAA"))).emplaceValue(1);

    TrieParser p(e, std::move(trie));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Trie parser, line continuation literal") {
    TrieParserTestEnvironment e;
    e.appendSource(L("A\\\nBC"));

    auto trie = std::make_shared<Trie>();
    trie->emplaceDescendants(String(L("AB"))).emplaceValue(1);

    TrieParser p(e, std::move(trie), LineContinuationTreatment::LITERAL);
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Trie parser, line continuation remove") {
    TrieParserTestEnvironment e;
    e.appendSource(L("A\\\nBC"));

    auto trie = std::make_shared<Trie>();
    trie->emplaceDescendants(String(L("AB"))).emplaceValue(1);

    TrieParser p(e, std::move(trie), LineContinuationTreatment::REMOVE);
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == 1);
    CHECK(e.position() == 2);
}

TEST_CASE("Trie parser, reset") {
    TrieParserTestEnvironment e;
    e.appendSource(L("AAA"));

    auto trie = std::make_shared<Trie>();
    trie->emplaceDescendants(String(L("AA"))).emplaceValue(1);

    TrieParser p(e, std::move(trie));
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == 1);
    CHECK(e.position() == 2);

    p.reset();
    CHECK_THROWS_AS(p.parse(), IncompleteParse);

    e.setIsEof();
    CHECK_FALSE(p.parse().hasValue());
}

}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
