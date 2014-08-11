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
#include "common/String.hh"
#include "common/TypeTagTestHelper.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/CharPredicates.hh"
#include "language/parser/Converter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/FailingParser.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/PipelineParser.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParserTestHelper.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::EnumSet;
using sesh::common::String;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::CharParser;
using sesh::language::parser::Converter;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::FailingParser;
using sesh::language::parser::Keyword;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::Parser;
using sesh::language::parser::PipelineParser;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::TokenParserStub;
using sesh::language::parser::TokenType;
using sesh::language::parser::isRawStringChar;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::WordComponent;

class PipelineParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

PipelineParser::TokenParserPointer newTokenParser(Environment &e) {
    return PipelineParser::TokenParserPointer(
            new TokenParserStub(e, EnumSet<TokenType>().set()));
}

class PipelineParserStubBase : public PipelineParser {
    using PipelineParser::PipelineParser;
    TokenParserPointer createTokenParser() const override {
        return newTokenParser(environment());
    }
}; // class PipelineParserStubBase

void checkWord(const Word *w, const String &s) {
    REQUIRE(w != nullptr);
    REQUIRE(w->components().size() == 1);
    const RawString *rs =
            dynamic_cast<const RawString *>(w->components().at(0).get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == s);
}

class CommandStub : public Command {
    void print(Printer &) const override { throw "unexpected print"; }
};

void checkCommandStub(const Command *c) {
    CHECK(dynamic_cast<const CommandStub *>(c) != nullptr);
}

template<Char expected>
constexpr bool is(const Environment &, Char c) noexcept {
    return c == expected;
}

class CommandParserStub : public Parser<std::unique_ptr<Command>> {
private:
    CharParser mSpaceParser, mCParser;
    std::unique_ptr<Command> mResultCommand;
public:
    CommandParserStub(Environment &e) :
            Parser<std::unique_ptr<Command>>(e),
            mSpaceParser(e, is<L(' ')>),
            mCParser(e, is<L('C')>) { }
private:
    void parseImpl() override {
        mSpaceParser.parse();
        if (mCParser.parse() != nullptr) {
            mResultCommand.reset(new CommandStub);
            result() = &mResultCommand;
        }
    }
    void resetImpl() noexcept override {
        mSpaceParser.reset();
        mCParser.reset();
        mResultCommand.reset();
        Parser<std::unique_ptr<Command>>::resetImpl();
    }
};

class NegatedPipelineParserStub : public PipelineParserStubBase {
    using PipelineParserStubBase::PipelineParserStubBase;
    CommandParserPointer createCommandParser(TokenParserPointer &&tp) const
            override {
        REQUIRE(tp != nullptr);
        CHECK(tp->state() == State::UNSTARTED);
        return CommandParserPointer(new CommandParserStub(environment()));
    }
}; // class PipelineParserStub

TEST_CASE("Pipeline parser, construction and assignment") {
    class PipelineParserStub : public PipelineParser {
        using PipelineParser::PipelineParser;
    public:
        TokenParserPointer createTokenParser() const override {
            throw "unexpected createTokenParser";
        }
        CommandParserPointer createCommandParser(TokenParserPointer &&) const
                override {
            throw "unexpected createCommandParser";
        }
    }; // class PipelineParserStub

    PipelineParserTestEnvironment e;
    PipelineParserStub p1(e);
    PipelineParserStub p2(e, newTokenParser(e));
    PipelineParserStub(std::move(p2));
    p2 = std::move(p1);
}

TEST_CASE("Pipeline parser, failing token parser") {
    class PipelineParserStub : public PipelineParserStubBase {
        using PipelineParserStubBase::PipelineParserStubBase;
        CommandParserPointer createCommandParser(TokenParserPointer &&) const
                override {
            throw "unexpected createCommandParser";
        }
    }; // class PipelineParserStub

    PipelineParserTestEnvironment e;
    PipelineParserStub p(e);
    e.setIsEof();
    CHECK(p.parse() == nullptr);
}

TEST_CASE("Pipeline parser, non-keyword token, single command") {
    class PipelineParserStub : public PipelineParserStubBase {
        using PipelineParserStubBase::PipelineParserStubBase;
        TokenParserPointer createTokenParser() const override {
            throw "unexpected createTokenParser";
        }
        CommandParserPointer createCommandParser(TokenParserPointer &&tp) const
                override {
            REQUIRE(tp != nullptr);
            CHECK(tp->state() == State::FINISHED);
            REQUIRE(tp->parse() != nullptr);
            REQUIRE(tp->parse()->tag() ==
                    tp->parse()->tag<std::unique_ptr<Word>>());
            checkWord(
                    tp->parse()->value<std::unique_ptr<Word>>().get(), L("A"));
            CHECK(environment().position() == 1);
            return CommandParserPointer(new CommandParserStub(environment()));
        }
    }; // class PipelineParserStub

    PipelineParserTestEnvironment e;
    e.appendSource(L("A C"));
    e.setIsEof();

    auto tp = newTokenParser(e);
    REQUIRE(tp->parse() != nullptr);
    REQUIRE(tp->parse()->tag() == tp->parse()->tag<std::unique_ptr<Word>>());
    checkWord(tp->parse()->value<std::unique_ptr<Word>>().get(), L("A"));
    CHECK(e.position() == 1);

    PipelineParserStub pp(e, std::move(tp));
    REQUIRE(pp.parse() != nullptr);
    REQUIRE(*pp.parse() != nullptr);

    const Pipeline &p = **pp.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    REQUIRE(p.commands().size() == 1);
    checkCommandStub(p.commands().at(0).get());
}

TEST_CASE("Pipeline parser, keyword token, single command") {
    class PipelineParserStub : public PipelineParserStubBase {
        using PipelineParserStubBase::PipelineParserStubBase;
        CommandParserPointer createCommandParser(TokenParserPointer &&tp) const
                override {
            REQUIRE(tp != nullptr);
            CHECK(tp->state() == State::FINISHED);
            REQUIRE(tp->parse() != nullptr);
            REQUIRE(tp->parse()->tag() == tp->parse()->tag<Keyword>());
            CHECK(tp->parse()->value<Keyword>() ==
                    Keyword::keywordLeftBrace());
            return CommandParserPointer(new CommandParserStub(environment()));
        }
    }; // class PipelineParserStub

    PipelineParserTestEnvironment e;
    e.appendSource(L("{ C"));
    e.setIsEof();

    PipelineParserStub parser(e);
    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    REQUIRE(p.commands().size() == 1);
    checkCommandStub(p.commands().at(0).get());
}

TEST_CASE("Pipeline parser, negated single command") {
    PipelineParserTestEnvironment e;
    e.appendSource(L("! C"));
    e.setIsEof();

    NegatedPipelineParserStub parser(e);
    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    REQUIRE(p.commands().size() == 1);
    checkCommandStub(p.commands().at(0).get());
}

TEST_CASE("Pipeline parser, negated three commands") {
    PipelineParserTestEnvironment e;
    e.appendSource(L("! C| C| C; "));

    NegatedPipelineParserStub parser(e);
    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);
    CHECK(e.position() == 9);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    REQUIRE(p.commands().size() == 3);
    checkCommandStub(p.commands().at(0).get());
    checkCommandStub(p.commands().at(1).get());
    checkCommandStub(p.commands().at(2).get());
}

TEST_CASE("Pipeline parser, failure in first command") {
    PipelineParserTestEnvironment e;
    e.appendSource(L("! X"));

    NegatedPipelineParserStub parser(e);
    CHECK(parser.parse() == nullptr);
}

TEST_CASE("Pipeline parser, failure in third command") {
    PipelineParserTestEnvironment e;
    e.appendSource(L("! C| C| X"));

    NegatedPipelineParserStub parser(e);
    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);
    CHECK(e.position() == 6);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    REQUIRE(p.commands().size() == 2);
    checkCommandStub(p.commands().at(0).get());
    checkCommandStub(p.commands().at(1).get());
}

TEST_CASE("Pipeline parser, pipe followed by linebreak") {
    PipelineParserTestEnvironment e;
    e.appendSource(L("! C| #comment\n C|\n\n C; "));

    NegatedPipelineParserStub parser(e);
    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);
    CHECK(e.position() == 21);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    REQUIRE(p.commands().size() == 3);
    checkCommandStub(p.commands().at(0).get());
    checkCommandStub(p.commands().at(1).get());
    checkCommandStub(p.commands().at(2).get());
}

TEST_CASE("Pipeline parser, reset") {
    class PipelineParserStub : public PipelineParserStubBase {
        using PipelineParserStubBase::PipelineParserStubBase;
        CommandParserPointer createCommandParser(TokenParserPointer &&) const
                override {
            return CommandParserPointer(new CommandParserStub(environment()));
        }
    }; // class PipelineParserStub

    PipelineParserTestEnvironment e;
    PipelineParserStub parser(e);

    e.appendSource(L("! X"));
    CHECK(parser.parse() == nullptr);

    parser.reset();
    e.setPosition(e.length());
    e.appendSource(L("X C; "));

    REQUIRE(parser.parse() != nullptr);
    REQUIRE(*parser.parse() != nullptr);

    const Pipeline &p = **parser.parse();
    CHECK(p.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    REQUIRE(p.commands().size() == 1);
    checkCommandStub(p.commands().at(0).get());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
