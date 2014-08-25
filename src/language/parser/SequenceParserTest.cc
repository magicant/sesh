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
#include "common/TypeTagTestHelper.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/Converter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/FailingParser.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/SequenceParser.hh"
#include "language/parser/SourceEnvironment.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParserTestHelper.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Pipeline.hh"

namespace {

using sesh::common::Char;
using sesh::common::enumSetOf;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::CharParser;
using sesh::language::parser::Converter;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::FailingParser;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::Keyword;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::Parser;
using sesh::language::parser::ParserBase;
using sesh::language::parser::SequenceParser;
using sesh::language::parser::SourceEnvironment;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::Token;
using sesh::language::parser::TokenParserStub;
using sesh::language::parser::TokenType;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Pipeline;

constexpr bool isSpace(const Environment &, Char c) noexcept {
    return c == L(' ');
}

class SequenceParserStub : public SequenceParser {

    using SequenceParser::SequenceParser;

private:

    TokenParserPointer createTokenParser() const override {
        return TokenParserPointer(new TokenParserStub(
                environment(), enumSetOf(TokenType::KEYWORD)));
    }

    AndOrListParserPointer createAndOrListParser(TokenParserPointer &&) const
            override {
        throw "unexpected createAndOrListParser";
    }

}; // class SequenceParserStub

class AndOrListParserStub : public Converter<
        CharParser, std::pair<std::unique_ptr<AndOrList>, bool>> {

private:

    bool mIsSeparated;

    Result mResult;

public:

    explicit AndOrListParserStub(Environment &e, bool isSeparated = false) :
            Converter(e, e, isSpace),
            mIsSeparated(isSeparated),
            mResult() { }

private:

    void convert(Char &&) override {
        mResult.first.reset(new AndOrList(Pipeline()));
        mResult.second = mIsSeparated;
        result() = &mResult;
    }

    void resetImpl() noexcept override {
        mResult.first.reset();
        Converter::resetImpl();
    }

};

void checkTokenParserLeftBrace(Parser<Token> &p) {
    CHECK(p.state() == ParserBase::State::FINISHED);
    REQUIRE(p.parse() != nullptr);
    REQUIRE(p.parse()->tag() == p.parse()->tag<Keyword>());
    CHECK(p.parse()->value<Keyword>() == Keyword::keywordLeftBrace());
}

class SequenceParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

TEST_CASE("Sequence parser, construction and assignment") {
    SourceEnvironment e;
    SequenceParserStub p1(e);
    SequenceParserStub p2(std::move(p1));
    p1 = std::move(p2);
}

TEST_CASE("Sequence parser, closing keyword") {
    SequenceParserTestEnvironment e;
    SequenceParserStub p(e);

    e.appendSource(L("}"));
    e.setIsEof();
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().empty());
    REQUIRE(p.parse()->second.hasValue());
    CHECK(p.parse()->second.value().keyword() == Keyword::keywordRightBrace());
    CHECK(p.parse()->second.value().position() == 0);
}

TEST_CASE("Sequence parser, non-closing keyword") {
    class SequenceParserImpl : public SequenceParserStub {
        using SequenceParserStub::SequenceParserStub;
        AndOrListParserPointer createAndOrListParser(TokenParserPointer &&p)
                const override {
            REQUIRE(p != nullptr);
            checkTokenParserLeftBrace(*p);
            return AndOrListParserPointer(
                    new AndOrListParserStub(environment()));
        }
    };

    SequenceParserTestEnvironment e;
    SequenceParserImpl p(e);

    e.appendSource(L("{ "));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 1);
    CHECK_FALSE(p.parse()->second.hasValue());
    CHECK(e.position() == 2);
}

TEST_CASE("Sequence parser, closing keyword after and-or lists") {
    class SequenceParserImpl : public SequenceParserStub {
        using SequenceParserStub::SequenceParserStub;
        AndOrListParserPointer createAndOrListParser(TokenParserPointer &&p)
                const override {
            REQUIRE(p != nullptr);
            checkTokenParserLeftBrace(*p);
            return AndOrListParserPointer(
                    new AndOrListParserStub(environment(), true));
        }
    };

    SequenceParserTestEnvironment e;
    SequenceParserImpl p(e);

    e.appendSource(L("{ "));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);

    e.appendSource(L("{ };"));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 2);
    REQUIRE(p.parse()->second.hasValue());
    CHECK(p.parse()->second.value().keyword() == Keyword::keywordRightBrace());
    CHECK(p.parse()->second.value().position() == 4);
    CHECK(e.position() == 5);
}

TEST_CASE("Sequence parser, failure in and-or list parser") {
    class SequenceParserImpl : public SequenceParserStub {
        using SequenceParserStub::SequenceParserStub;
        AndOrListParserPointer createAndOrListParser(TokenParserPointer &&)
                const override {
            return AndOrListParserPointer(
                    new FailingParser<std::pair<AndOrListPointer, bool>>(
                            environment()));
        }
    };

    SequenceParserTestEnvironment e;
    SequenceParserImpl p(e);

    e.appendSource(L("{ "));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 0);
    CHECK_FALSE(p.parse()->second.hasValue());
    CHECK(e.position() == 1); // better be 0?
}

TEST_CASE("Sequence parser, stop after non-separated and-or list") {
    class SequenceParserImpl : public SequenceParserStub {
        using SequenceParserStub::SequenceParserStub;
        mutable unsigned mTrueCount = 2;
        AndOrListParserPointer createAndOrListParser(TokenParserPointer &&p)
                const override {
            REQUIRE(p != nullptr);
            checkTokenParserLeftBrace(*p);
            return AndOrListParserPointer(
                    new AndOrListParserStub(environment(), mTrueCount-- > 0));
        }
    };

    SequenceParserTestEnvironment e;
    SequenceParserImpl p(e);

    e.appendSource(L("{ { { { { {"));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 3);
    CHECK_FALSE(p.parse()->second.hasValue());
    CHECK(e.position() == 6);
}

TEST_CASE("Sequence parser, reset") {
    class AndOrListParserStub2 : public Converter<
            CharParser, std::pair<std::unique_ptr<AndOrList>, bool>> {
    private:
        Result mResult;
    public:
        explicit AndOrListParserStub2(Environment &e) :
                Converter(e, e, isSpace), mResult() { }
    private:
        void convert(Char &&) override {
            mResult.first.reset(new AndOrList(Pipeline()));
            mResult.second =
                    environment().position() != environment().length();
            result() = &mResult;
        }
    };

    class SequenceParserImpl : public SequenceParserStub {
        using SequenceParserStub::SequenceParserStub;
        AndOrListParserPointer createAndOrListParser(TokenParserPointer &&p)
                const override {
            REQUIRE(p != nullptr);
            checkTokenParserLeftBrace(*p);
            return AndOrListParserPointer(
                    new AndOrListParserStub2(environment()));
        }
    };

    SequenceParserTestEnvironment e;
    SequenceParserImpl p(e);

    e.appendSource(L("{ "));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 1);
    CHECK_FALSE(p.parse()->second.hasValue());
    CHECK(e.position() == 2);

    p.reset();
    e.appendSource(L("{ { } "));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->first.andOrLists().size() == 2);
    REQUIRE(p.parse()->second.hasValue());
    CHECK(p.parse()->second.value().keyword() == Keyword::keywordRightBrace());
    CHECK(p.parse()->second.value().position() == 6);
    CHECK(e.position() == 7);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
