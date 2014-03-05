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
#include "common/EnumSet.hh"
#include "language/parser/CommandParser.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParser.hh"
#include "language/parser/TokenParserTestHelper.hh"

namespace {

using sesh::common::EnumSet;
using sesh::common::enumSetOf;
using sesh::language::parser::CLocaleEnvironment;
using sesh::language::parser::CommandParser;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::Parser;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::TokenParser;
using sesh::language::parser::TokenParserStub;
using sesh::language::parser::TokenType;
using sesh::language::syntax::Command;
using sesh::language::syntax::Printer;

class CommandParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public CLocaleEnvironment {
};

class CommandStub : public Command {
    void print(Printer &) const override { throw "unexpected print"; }
};

class SimpleCommandParserStub : public Parser<std::unique_ptr<Command>> {

    using Parser<std::unique_ptr<Command>>::Parser;

    std::unique_ptr<Command> mCommand;

    void parseImpl() override {
        mCommand.reset(new CommandStub);
        result() = &mCommand;
    }

    void resetImpl() noexcept override {
        mCommand.reset();
        Parser::resetImpl();
    }

}; // class SimpleCommandParserStub

class CommandParserStub : public CommandParser {

    using CommandParser::CommandParser;

    TokenParserPointer createTokenParser() const override {
        return TokenParserPointer(new TokenParserStub(
                environment(), EnumSet<TokenType>().set()));
    }

    CommandParserPointer createSimpleCommandParser(TokenParserPointer &&p)
            const override {
        CHECK(p != nullptr);
        return CommandParserPointer(
                new SimpleCommandParserStub(environment()));
    }

}; // class CommandParserStub

std::unique_ptr<TokenParser> createTokenParser(
        Environment &e, EnumSet<TokenType> types) {
    return std::unique_ptr<TokenParser>(new TokenParserStub(e, types));
}

TEST_CASE("Command parser, construction and assignment") {
    CommandParserTestEnvironment e;
    CommandParserStub p(e, createTokenParser(e, EnumSet<TokenType>()));
    p = CommandParserStub(std::move(p));
}

TEST_CASE("Command parser, simple command, unparsed token") {
    CommandParserTestEnvironment e;
    e.appendSource(L("A"));
    e.setIsEof();

    CommandParserStub p(e, createTokenParser(e, enumSetOf(TokenType::WORD)));
    REQUIRE(p.parse() != nullptr);
    CHECK(dynamic_cast<CommandStub *>(p.parse()->get()) != nullptr);
}

TEST_CASE("Command parser, simple command, pre-parsed token") {
    CommandParserTestEnvironment e;
    e.appendSource(L("A"));
    e.setIsEof();

    auto tp = createTokenParser(e, enumSetOf(TokenType::WORD));
    CHECK(tp->parse());

    CommandParserStub cp(e, std::move(tp));
    REQUIRE(cp.parse() != nullptr);
    CHECK(dynamic_cast<CommandStub *>(cp.parse()->get()) != nullptr);
}

TEST_CASE("Command parser, simple command, null parser") {
    CommandParserTestEnvironment e;
    e.appendSource(L("A"));
    e.setIsEof();

    CommandParserStub p(e);
    REQUIRE(p.parse() != nullptr);
    CHECK(dynamic_cast<CommandStub *>(p.parse()->get()) != nullptr);
}

TEST_CASE("Command parser, simple command, reset") {
    CommandParserTestEnvironment e;
    e.appendSource(L("A"));
    e.setIsEof();

    CommandParserStub p(e);
    CHECK(p.parse() != nullptr);

    e.appendSource(L("B"));

    p.reset();
    REQUIRE(p.parse() != nullptr);
    CHECK(dynamic_cast<CommandStub *>(p.parse()->get()) != nullptr);
}

// TODO compound commands
// TODO function definition command

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
