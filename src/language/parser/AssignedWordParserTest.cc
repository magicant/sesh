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

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/AssignedWordParser.hh"
#include "language/parser/Converter.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/FailingParser.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::AssignedWordParser;
using sesh::language::parser::Converter;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::FailingParser;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::StringParser;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::WordComponent;

using ComponentPointer = Word::ComponentPointer;
using WordPointer = AssignedWordParser::WordPointer;
using WordParserPointer = AssignedWordParser::WordParserPointer;

class AssignedWordParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

class WordParserStub : public Converter<StringParser, WordPointer> {

    using Converter<StringParser, WordPointer>::Converter;

    WordPointer mResultWord;

    void convert(String &&s) override {
        mResultWord.reset(new Word);
        if (!s.empty())
            mResultWord->addComponent(
                    Word::ComponentPointer(new RawString(std::move(s))));
        result() = &mResultWord;
    }

    void resetImpl() noexcept override {
        mResultWord.reset();
        Converter<StringParser, WordPointer>::resetImpl();
    }

}; // class WordParserStub

constexpr bool isWordCharStub(const Environment &, Char c) noexcept {
    return c != L(' ') && c != L(':');
}

WordParserPointer wordParserStub(Environment &e) {
    return WordParserPointer(new WordParserStub(e, e, e, isWordCharStub));
}

void checkComponents(
        const std::vector<ComponentPointer> &actual,
        const std::vector<String> &expected) {
    CHECK(actual.size() == expected.size());

    for (std::size_t i = 0; i < expected.size(); ++i) {
        const RawString *rs =
                dynamic_cast<const RawString *>(actual.at(i).get());
        REQUIRE(rs != nullptr);
        CHECK(rs->value() == expected.at(i));
    }
}

TEST_CASE("Assigned word parser, construction and assignment") {
    AssignedWordParserTestEnvironment e;
    AssignedWordParser p1(
            WordParserPointer(new FailingParser<WordPointer>(e)));
    AssignedWordParser p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Assigned word parser, parse") {
    AssignedWordParserTestEnvironment e;
    AssignedWordParser p(wordParserStub(e));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("A"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("B"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(":"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("C"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("D"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(":"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("E"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("F"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(" "));

    REQUIRE(p.parse() != nullptr);
    REQUIRE(p.parse()->get() != nullptr);
    checkComponents(
            p.parse()->get()->components(),
            {L("AB"), L(":"), L("CD"), L(":"), L("EF")});
}

TEST_CASE("Assigned word parser, reset") {
    AssignedWordParserTestEnvironment e;
    AssignedWordParser p(wordParserStub(e));

    e.appendSource(L("X:Y "));

    REQUIRE(p.parse() != nullptr);
    REQUIRE(p.parse()->get() != nullptr);
    checkComponents(
            p.parse()->get()->components(), {L("X"), L(":"), L("Y")});

    p.reset();
    e.appendSource(L(":0:"));
    e.setIsEof();
    e.setPosition(e.position() + 1);

    REQUIRE(p.parse() != nullptr);
    REQUIRE(p.parse()->get() != nullptr);
    checkComponents(
            p.parse()->get()->components(), {L(":"), L("0"), L(":")});
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
