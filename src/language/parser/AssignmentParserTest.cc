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
#include "language/parser/AssignmentParser.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/Predicate.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::AssignmentParser;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::CharParser;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NormalParser;
using sesh::language::parser::Predicate;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::Printer;
using sesh::language::syntax::Word;
using sesh::language::syntax::WordComponent;

class AssignmentParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

class StringParserStub : public NormalParser<String> {

private:

    String mResult;

public:

    template<typename... Arg>
    StringParserStub(Environment &e, Arg &&... arg) :
            NormalParser<String>(e),
            mResult(std::forward<Arg>(arg)...) { }

private:

    void parseImpl() override {
        result() = mResult;
    }

}; // class StringParserStub

class WordComponentStub : public WordComponent {

    void print(Printer &) const override { throw "unexpected print"; }

};

class AssignedWordParserStub : public NormalParser<std::unique_ptr<Word>> {

    using NormalParser<std::unique_ptr<Word>>::NormalParser;

    void parseImpl() override {
        result().emplace(new Word);
        result().value()->components().emplace_back(new WordComponentStub);
    }

}; // class AssignedWordParserStub

class AssignmentParserStub1 : public AssignmentParser {

    using AssignmentParser::AssignmentParser;

    StringParserPointer createStringParser(
            Predicate<Char> &&, LineContinuationTreatment)
            const override {
        throw "unexpected createStringParser";
    }

    CharParserPointer createCharParser(
            Predicate<Char> &&, LineContinuationTreatment)
            const override {
        throw "unexpected createCharParser";
    }

    WordParserPointer createAssignedWordParser(Predicate<Char> &&)
            const override {
        throw "unexpected createAssignedWordParser";
    }

}; // class AssignmentParserStub

class AssignmentParserStub2 : public AssignmentParserStub1 {

private:

    String mName;

public:

    AssignmentParserStub2(Environment &e, String name) :
            AssignmentParserStub1(e), mName(std::move(name)) { }

private:

    StringParserPointer createStringParser(
            Predicate<Char> &&p, LineContinuationTreatment t) const override {
        CHECK_FALSE(p(environment(), L('=')));
        CHECK(t == LineContinuationTreatment::REMOVE);
        return StringParserPointer(new StringParserStub(environment(), mName));
    }

}; // class AssignmentParserStub2

class AssignmentParserStub3 : public AssignmentParserStub2 {

    using AssignmentParserStub2::AssignmentParserStub2;

    CharParserPointer createCharParser(
            Predicate<Char> &&p, LineContinuationTreatment t) const override {
        using sesh::language::parser::CharParser;
        return CharParserPointer(
                new CharParser(environment(), std::move(p), t));
    }

}; // class AssignmentParserStub3

class AssignmentParserStub4 : public AssignmentParserStub3 {

    using AssignmentParserStub3::AssignmentParserStub3;

    WordParserPointer createAssignedWordParser(Predicate<Char> &&p)
            const override {
        CHECK_FALSE(p(environment(), L(':')));
        return WordParserPointer(new AssignedWordParserStub(environment()));
    }

}; // class AssignmentParserStub4

TEST_CASE("Assignment parser, construction") {
    AssignmentParserTestEnvironment e;
    AssignmentParserStub1 p(e);
    AssignmentParserStub1(std::move(p));
}

TEST_CASE("Assignment parser, empty name") {
    AssignmentParserTestEnvironment e;
    AssignmentParserStub2 p(e, L(""));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Assignment parser, name starting with digit") {
    AssignmentParserTestEnvironment e;
    AssignmentParserStub2 p(e, L("0"));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Assignment parser, no equal") {
    AssignmentParserTestEnvironment e;
    AssignmentParserStub3 p(e, L("foo"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("|"));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Assignment parser, success and reset") {
    AssignmentParserTestEnvironment e;
    AssignmentParserStub4 p(e, L("foo"));
    e.appendSource(L("=|"));

    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == 1);

    std::unique_ptr<Assignment> a = std::move(p.parse().value());
    REQUIRE(a != nullptr);
    CHECK(a->variableName() == L("foo"));
    REQUIRE(a->value().components().size() == 1);

    const WordComponent *wc = a->value().components().at(0).get();
    CHECK(dynamic_cast<const WordComponentStub *>(wc) != nullptr);

    p.reset();
    CHECK_FALSE(p.parse().hasValue());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */