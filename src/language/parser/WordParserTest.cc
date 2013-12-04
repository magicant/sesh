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

#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/WordParser.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::ParserBase;
using sesh::language::parser::Predicate;
using sesh::language::parser::WordParser;
using sesh::language::syntax::Printer;
using sesh::language::syntax::WordComponent;

class ComponentStub : public WordComponent {

    void print(Printer &) const override { throw "unexpected print"; }

};

class ComponentParserStub :
        public Parser<std::unique_ptr<WordComponent>>, protected ParserBase {

public:

    ComponentParserStub(Environment &e) noexcept : Parser(), ParserBase(e) { }

    std::unique_ptr<WordComponent> parse() override {
        if (environment().end() - environment().current() < 2)
            return environment().isEof() ? nullptr : throw NeedMoreSource();
        environment().current() += 2;
        return std::unique_ptr<WordComponent>(new ComponentStub);
    }

};

WordParser::ComponentParserPointer failCreateComponentParser(Environment &) {
    throw "unexpected";
}

WordParser::ComponentParserPointer createNullComponentParser(Environment &) {
    return nullptr;
}

WordParser::ComponentParserPointer createComponentParserStub(Environment &e) {
    return WordParser::ComponentParserPointer(new ComponentParserStub(e));
}

TEST_CASE("Word parser construction") {
    BasicEnvironmentStub e;
    WordParser p(e, failCreateComponentParser);
}

TEST_CASE("Word parser, empty word, null component parser") {
    BasicEnvironmentStub e;
    WordParser p(e, createNullComponentParser);
    auto result = p.parse();

    REQUIRE(result != nullptr);
    CHECK(result->components().empty());
    CHECK(e.current() == e.end());
}

TEST_CASE("Word parser, empty word, null component") {
    BasicEnvironmentStub e;
    e.setIsEof();

    WordParser p(e, createComponentParserStub);
    auto result = p.parse();

    REQUIRE(result != nullptr);
    CHECK(result->components().empty());
    CHECK(e.current() == e.end());
}

TEST_CASE("Word parser, one component") {
    BasicEnvironmentStub e;
    e.appendSource(L("AA!"));
    e.setIsEof();

    WordParser p(e, createComponentParserStub);
    auto result = p.parse();

    REQUIRE(result != nullptr);
    CHECK(result->components().size() == 1);
    CHECK(e.current() == e.begin() + 2);
    ComponentStub *rss =
            dynamic_cast<ComponentStub *>(result->components().at(0).get());
    CHECK(rss != nullptr);
}

TEST_CASE("Word parser, three components") {
    BasicEnvironmentStub e;
    e.appendSource(L("banana"));
    e.setIsEof();

    WordParser p(e, createComponentParserStub);
    auto result = p.parse();

    REQUIRE(result != nullptr);
    CHECK(result->components().size() == 3);
    CHECK(e.current() == e.end());

    ComponentStub *rss;
    rss = dynamic_cast<ComponentStub *>(result->components().at(0).get());
    CHECK(rss != nullptr);
    rss = dynamic_cast<ComponentStub *>(result->components().at(1).get());
    CHECK(rss != nullptr);
    rss = dynamic_cast<ComponentStub *>(result->components().at(2).get());
    CHECK(rss != nullptr);
}

TEST_CASE("Word parser, need more source") {
    BasicEnvironmentStub e;
    WordParser p(e, createComponentParserStub);

    e.appendSource(L("!"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("!"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("X"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.setIsEof();
    auto result = p.parse();
    REQUIRE(result != nullptr);
    CHECK(result->components().size() == 1);
    CHECK(e.current() == e.begin() + 2);
    ComponentStub *rss =
            dynamic_cast<ComponentStub *>(result->components().at(0).get());
    CHECK(rss != nullptr);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
