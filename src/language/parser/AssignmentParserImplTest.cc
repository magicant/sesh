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

#include <memory>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/AssignmentParserImpl.tcc"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Skipper.hh"
#include "language/parser/WordParserTestHelper.hh"
#include "language/source/SourceBuffer.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::AssignmentParserImpl;
using sesh::language::parser::CLocaleEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::Skipper;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

class TestTypes {
public:
    using Skipper = sesh::language::parser::Skipper;
    using WordParser = sesh::language::parser::WordParserStub;
};

using AssignmentParser = AssignmentParserImpl<TestTypes>;
using AssignmentPointer = AssignmentParser::AssignmentPointer;
using WordPointer = AssignmentParser::WordPointer;
using Result = AssignmentParser::Result;

void checkWord(const Word *w, const String &value) {
    REQUIRE(w != nullptr);
    CHECK(w->components().size() == 1);
    RawString *rs =
            dynamic_cast<RawString *>(w->components().at(0).get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == value);
}

void checkWordResult(
        const AssignmentParser::Result &result, const String &value) {
    REQUIRE(result.index() == result.index<WordPointer>());
    checkWord(result.value<WordPointer>().get(), value);
}

void checkAssignment(
        const Assignment *a, const String &name, const String &value) {
    REQUIRE(a != nullptr);
    CHECK(a->variableName() == name);
    checkWord(&a->value(), value);
}

void checkAssignmentResult(
        const AssignmentParser::Result &result,
        const String &variableName,
        const String &wordValue) {
    REQUIRE(result.index() == result.index<AssignmentPointer>());
    checkAssignment(
            result.value<AssignmentPointer>().get(), variableName, wordValue);
}

TEST_CASE("Assignment parser construction") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);
    AssignmentParser(std::move(p));
}

TEST_CASE("Assignment parser, empty word") {
    CLocaleEnvironmentStub e;
    e.setIsEof();

    AssignmentParser p(e);

    auto result = p.parse();
    CHECK(e.current() == e.end());
    checkWordResult(result, String());
}

TEST_CASE("Assignment parser, empty variable name") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("=a "));
    e.setIsEof();

    auto result = p.parse();
    CHECK(e.current() == e.begin() + 2);
    checkWordResult(result, L("=a"));
}

TEST_CASE("Assignment parser, valid variable name, empty value") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("var"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("iable= "));
    auto result = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkAssignmentResult(result, L("variable"), L(""));
}

TEST_CASE("Assignment parser, valid variable name, non-empty value") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);
    e.appendSource(L("variable"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("="));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("="));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.setIsEof();
    auto result = p.parse();
    CHECK(e.current() == e.end());
    checkAssignmentResult(result, L("variable"), L("="));
}

TEST_CASE("Assignment parser, invalid variable name") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);

    e.appendSource(L("invalid#name=value "));
    e.setIsEof();
    auto result = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkWordResult(result, L("invalid#name=value"));
}

TEST_CASE("Assignment parser, escaped =") {
    CLocaleEnvironmentStub e;
    AssignmentParser p(e);

    e.appendSource(L("name\\=value "));
    e.setIsEof();
    auto result = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkWordResult(result, L("name\\=value"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
