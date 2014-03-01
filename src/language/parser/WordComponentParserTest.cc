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

#include <memory>
#include <utility>
#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/WordComponentParser.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::Maybe;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NormalParser;
using sesh::language::parser::Parser;
using sesh::language::parser::Predicate;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::WordComponentParser;
using sesh::language::syntax::Printer;
using sesh::language::syntax::WordComponent;

class RawStringStub : public WordComponent {

    bool appendConstantValue(String &) const override { throw "unexpected"; }
    void print(Printer &) const override { throw "unexpected print"; }

};

class RawStringParserStub :
        public NormalParser<std::unique_ptr<WordComponent>> {

public:

    using NormalParser::NormalParser;

    void parseImpl() override {
        if (environment().length() - environment().position() < 2) {
            if (environment().isEof())
                return;
            else
                throw IncompleteParse();
        }
        environment().setPosition(environment().position() + 2);
        result().emplace(new RawStringStub);
    }

};

class WordComponentParserStub : public WordComponentParser {

    using WordComponentParser::WordComponentParser;

    ParserPointer createRawStringParser(
            Predicate<Char> &&isAcceptableChar,
            LineContinuationTreatment lct = LineContinuationTreatment::REMOVE)
            const override {
        CHECK(isAcceptableChar != nullptr);
        CHECK(lct == LineContinuationTreatment::REMOVE);
        return ParserPointer(new RawStringParserStub(environment()));
    }

}; // class WordComponentParser

class WordComponentParserTestEnvironment :
        public SourceTestEnvironment, public EofEnvironment {
};

bool fail(const Environment &, Char) {
    FAIL("unexpected predicate test");
    return true;
}

TEST_CASE("Word component parser, construction and assignment") {
    WordComponentParserTestEnvironment e;
    WordComponentParserStub p1(e, fail);
    WordComponentParserStub p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Word component parser, eof") {
    WordComponentParserTestEnvironment e;
    e.setIsEof();

    WordComponentParserStub p(e, fail);
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Word component parser, raw string") {
    WordComponentParserTestEnvironment e;
    WordComponentParserStub p(e, fail);

    e.appendSource(L("AA"));
    REQUIRE(p.parse().hasValue());
    CHECK(dynamic_cast<RawStringStub *>(p.parse().value().get()) != nullptr);
    CHECK(e.position() == e.length());
}

/*
TEST_CASE("Word component parser, incomplete parse at initial position") {
    WordComponentParserTestEnvironment e;
    WordComponentParserStub p(e, fail);
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("\\"));
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("\n"));
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("\\"));
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("\n"));
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("1"));
    REQUIRE_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("2"));
    REQUIRE(p.parse());

    e.checkSource(L("12"));
    CHECK(e.current() == e.end());
    CHECK(p.result() != nullptr);
    CHECK(dynamic_cast<RawStringStub *>(p.result().get()) != nullptr);
}
*/

// TODO other word component parser types

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
