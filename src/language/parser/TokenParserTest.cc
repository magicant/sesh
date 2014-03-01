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
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParserTestHelper.hh"
#include "language/parser/WordParserTestHelper.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::EnumSet;
using sesh::common::String;
using sesh::common::enumSetOf;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::Keyword;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::Token;
using sesh::language::parser::TokenParserStub;
using sesh::language::parser::TokenType;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;

using AssignmentPointer = std::unique_ptr<Assignment>;
using WordPointer = std::unique_ptr<Word>;

class TokenParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

void checkAssignment(const Token &r) {
    REQUIRE(r.index() == r.index<AssignmentPointer>());
    REQUIRE(r.value<AssignmentPointer>() != nullptr);
    CHECK(r.value<AssignmentPointer>()->variableName() == L("N"));
    CHECK(r.value<AssignmentPointer>()->value().components().empty());
}

void checkKeyword(const Token &r, const String &s) {
    REQUIRE(r.index() == r.index<Keyword>());
    CHECK(r.value<Keyword>().get() == s);
}

void checkWord(const Token &r, const String &s) {
    REQUIRE(r.index() == r.index<WordPointer>());
    REQUIRE(r.value<WordPointer>() != nullptr);
    REQUIRE(r.value<WordPointer>()->components().size() == 1);

    const RawString *rs = dynamic_cast<RawString *>(
            r.value<WordPointer>()->components()[0].get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == s);
}

TEST_CASE("Simple command word parser, construction and assignment") {
    TokenParserTestEnvironment e;
    TokenParserStub p1(e, EnumSet<TokenType>());
    TokenParserStub p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Simple command word parser, assignment") {
    TokenParserTestEnvironment e;
    e.appendSource(L("="));

    TokenParserStub p(e, enumSetOf(TokenType::ASSIGNMENT));
    REQUIRE(p.parse().hasValue());
    checkAssignment(p.parse().value());
}

TEST_CASE("Simple command word parser, assignment & keyword -> assignment") {
    TokenParserTestEnvironment e;
    e.appendSource(L("="));

    TokenParserStub p(e, enumSetOf(TokenType::ASSIGNMENT, TokenType::KEYWORD));
    REQUIRE(p.parse().hasValue());
    checkAssignment(p.parse().value());
}

TEST_CASE("Simple command word parser, keyword") {
    TokenParserTestEnvironment e;
    e.appendSource(L("!"));

    TokenParserStub p(e, enumSetOf(TokenType::KEYWORD));
    REQUIRE(p.parse().hasValue());
    checkKeyword(p.parse().value(), Keyword::EXCLAMATION);
}

TEST_CASE("Simple command word parser, assignment & keyword -> keyword") {
    TokenParserTestEnvironment e;
    e.appendSource(L("!"));

    TokenParserStub p(e, enumSetOf(TokenType::ASSIGNMENT, TokenType::KEYWORD));
    REQUIRE(p.parse().hasValue());
    checkKeyword(p.parse().value(), Keyword::EXCLAMATION);
}

TEST_CASE("Simple command word parser, keyword & word -> keyword") {
    TokenParserTestEnvironment e;
    e.appendSource(L("!"));

    TokenParserStub p(e, enumSetOf(TokenType::KEYWORD, TokenType::WORD));
    REQUIRE(p.parse().hasValue());
    checkKeyword(p.parse().value(), Keyword::EXCLAMATION);
}

TEST_CASE("Simple command word parser, word") {
    TokenParserTestEnvironment e;
    e.appendSource(L("="));

    TokenParserStub p(e, enumSetOf(TokenType::WORD));
    REQUIRE(p.parse().hasValue());
    checkWord(p.parse().value(), L("="));
}

TEST_CASE("Simple command word parser, all -> word") {
    TokenParserTestEnvironment e;
    e.appendSource(L("W"));

    auto types = enumSetOf(
            TokenType::ASSIGNMENT, TokenType::KEYWORD, TokenType::WORD);
    TokenParserStub p(e, types);
    REQUIRE(p.parse().hasValue());
    checkWord(p.parse().value(), L("W"));
}

TEST_CASE("Simple command word parser, all -> empty word") {
    TokenParserTestEnvironment e;
    e.setIsEof();

    auto types = enumSetOf(
            TokenType::ASSIGNMENT, TokenType::KEYWORD, TokenType::WORD);
    TokenParserStub p(e, types);
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Simple command word parser, blank before assignment") {
    TokenParserTestEnvironment e;
    e.appendSource(L("\t \t="));

    TokenParserStub p(e, enumSetOf(TokenType::ASSIGNMENT));
    REQUIRE(p.parse().hasValue());
    checkAssignment(p.parse().value());
}

TEST_CASE("Simple command word parser, blank before word") {
    TokenParserTestEnvironment e;
    e.appendSource(L(" \t ="));

    TokenParserStub p(e, enumSetOf(TokenType::WORD));
    REQUIRE(p.parse().hasValue());
    checkWord(p.parse().value(), L("="));
}

TEST_CASE("Simple command word parser, comment") {
    TokenParserTestEnvironment e;
    e.appendSource(L(" #=\n"));

    TokenParserStub p(e, enumSetOf(TokenType::WORD));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Simple command word parser, reset") {
    TokenParserTestEnvironment e;

    TokenParserStub p(e, enumSetOf(TokenType::WORD));
    e.appendSource(L(" W"));
    REQUIRE(p.parse().hasValue());
    checkWord(p.parse().value(), L("W"));

    p.reset(enumSetOf(TokenType::ASSIGNMENT));
    e.appendSource(L("\t="));
    REQUIRE(p.parse().hasValue());
    checkAssignment(p.parse().value());

    p.reset(enumSetOf(TokenType::WORD));
    e.appendSource(L("\\\n="));
    REQUIRE(p.parse().hasValue());
    checkWord(p.parse().value(), L("="));

    // TODO redirection
    // TODO alias substitution
}

// TODO redirection
// TODO alias substitution

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
