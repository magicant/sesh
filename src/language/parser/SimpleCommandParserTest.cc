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
#include "common/EnumSet.hh"
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "language/parser/DiagnosticEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/SimpleCommandParser.hh"
#include "language/parser/TokenParser.hh"
#include "language/parser/TokenParserTestHelper.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::EnumSet;
using sesh::common::ErrorLevel;
using sesh::common::Message;
using sesh::common::String;
using sesh::common::enumSetOf;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::DiagnosticTestEnvironment;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SimpleCommandParser;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::TokenParser;
using sesh::language::parser::TokenParserStub;
using sesh::language::parser::TokenType;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;

class SimpleCommandParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public CLocaleEnvironment,
        public LineContinuationEnvironment,
        public DiagnosticTestEnvironment {
}; // class SimpleCommandParserTestEnvironment

class TestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

class CLocaleTestEnvironment :
        public TestEnvironment, public CLocaleEnvironment {
};

class CLocaleDiagnosticTestEnvironment :
        public CLocaleTestEnvironment, public DiagnosticTestEnvironment {
};

std::unique_ptr<TokenParser> createTokenParser(
        Environment &e, EnumSet<TokenType> acceptableTokenTypes) {
    return std::unique_ptr<TokenParser>(
            new TokenParserStub(e, acceptableTokenTypes));
}

void checkWord(const std::unique_ptr<Word> &w, const String &value) {
    REQUIRE(w != nullptr);
    CHECK(w->isRawString());

    const auto &actualValue = w->maybeConstantValue();
    REQUIRE(actualValue.hasValue());
    CHECK(actualValue.value() == value);
}

TEST_CASE("Simple command parser, construction and assignment") {
    TestEnvironment e;
    SimpleCommandParser p1(e, createTokenParser(e, EnumSet<TokenType>()));
    SimpleCommandParser p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Simple command parser, empty") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(e, createTokenParser(e, EnumSet<TokenType>()));
    e.appendSource(L(";"));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Simple command parser, keyword") {
    CLocaleDiagnosticTestEnvironment e;
    SimpleCommandParser p(e,
            createTokenParser(e, enumSetOf(TokenType::KEYWORD)));
    e.appendSource(L("!"));
    e.setIsEof();
    CHECK(p.parse().hasValue());
    CHECK(e.position() == e.length());

    auto m = L("keyword `!' cannot be used as command name");
    e.checkMessages({{0, Message<>(m), ErrorLevel::ERROR}});
}

TEST_CASE("Simple command parser, assignment only") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(
            e, createTokenParser( e, enumSetOf(TokenType::ASSIGNMENT)));
    e.appendSource(L("="));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 1);
    CHECK(c.words().size() == 0);
    // TODO CHECK(c.redirections().size() == 0);
}

TEST_CASE("Simple command parser, word only") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(e, createTokenParser(e, enumSetOf(TokenType::WORD)));
    e.appendSource(L("="));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 0);
    CHECK(c.words().size() == 1);
    // TODO CHECK(c.redirections().size() == 0);
}

// TODO TEST_CASE("Simple command parser, redirection only") { }
// TODO TEST_CASE("Simple command parser, alias substitution") { }

TEST_CASE("Simple command parser, assignment -> assignment") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(
            e, createTokenParser(e, enumSetOf(TokenType::ASSIGNMENT)));
    e.appendSource(L("= ="));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 2);
    CHECK(c.words().size() == 0);
    // TODO CHECK(c.redirections().size() == 0);
}

TEST_CASE("Simple command parser, assignment -> word") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(
            e, createTokenParser(e, enumSetOf(TokenType::ASSIGNMENT)));
    e.appendSource(L("= A"));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 1);
    CHECK(c.words().size() == 1);
    // TODO CHECK(c.redirections().size() == 0);
}

// TODO TEST_CASE("Simple command parser, assignment -> redirection") { }
// TODO TEST_CASE("Simple command parser, assignment -> alias") { }

TEST_CASE("Simple command parser, word -> word") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(e, createTokenParser(e, enumSetOf(TokenType::WORD)));
    e.appendSource(L("W W"));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 0);
    CHECK(c.words().size() == 2);
    // TODO CHECK(c.redirections().size() == 0);
}

TEST_CASE("Simple command parser, no assignments after word") {
    CLocaleTestEnvironment e;
    SimpleCommandParser p(
            e, createTokenParser(e, enumSetOf(TokenType::ASSIGNMENT)));
    e.appendSource(L("= = W ="));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 2);
    CHECK(c.words().size() == 2);
    checkWord(c.words().at(0), L("W"));
    checkWord(c.words().at(1), L("="));
    // TODO CHECK(c.redirections().size() == 0);
}

// TODO TEST_CASE("Simple command parser, no assign after word w/ redir") { }

TEST_CASE("Simple command parser, need more source") {
    // The source string is the same as "no assignments after word".

    CLocaleTestEnvironment e;
    SimpleCommandParser p(
            e, createTokenParser(e, enumSetOf(TokenType::ASSIGNMENT)));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("="));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(" "));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("="));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(" "));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("W"));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(" "));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("="));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.setIsEof();

    REQUIRE(p.parse());
    CHECK(e.position() == e.length());
    REQUIRE(p.parse().value() != nullptr);
    const auto &c = dynamic_cast<SimpleCommand &>(*p.parse().value());
    CHECK(c.assignments().size() == 2);
    CHECK(c.words().size() == 2);
    checkWord(c.words().at(0), L("W"));
    checkWord(c.words().at(1), L("="));
    // TODO CHECK(c.redirections().size() == 0);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
