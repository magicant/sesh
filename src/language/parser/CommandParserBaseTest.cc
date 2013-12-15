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
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/CommandParserBase.hh"
#include "language/parser/DiagnosticEnvironmentTestHelper.hh"
#include "language/parser/Parser.hh"
#include "language/source/SourceBuffer.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::ErrorLevel;
using sesh::common::Message;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::CommandParserBase;
using sesh::language::parser::DiagnosticEnvironmentStub;
using sesh::language::parser::Parser;
using sesh::language::source::SourceBuffer;
using sesh::language::syntax::Command;
using sesh::language::syntax::Printer;

class CommandStub : public Command {
    void print(Printer &) const override { throw "unexpected print"; }
};

class CommandParserStub : public Parser<std::unique_ptr<Command>> {
    std::unique_ptr<Command> parse() override {
        return std::unique_ptr<Command>(new CommandStub);
    }
};

class CommandParser : public CommandParserBase {

    using CommandParserBase::CommandParserBase;

    CommandParserPointer createSimpleCommandParser() const override {
        return CommandParserPointer(new CommandParserStub);
    }

}; // class CommandParser

void checkCommandStub(const Command *c) {
    const CommandStub *sc = dynamic_cast<const CommandStub *>(c);
    CHECK(sc != nullptr);
}

void checkSyntaxError(
        String &&source, SourceBuffer::Size position, Message<> &&message) {
    DiagnosticEnvironmentStub e;
    e.appendSource(std::move(source));

    CommandParser p(e);
    std::unique_ptr<Command> c = p.parse();
    CHECK(c == nullptr);
    e.checkMessages({{
            e.begin() + position, std::move(message), ErrorLevel::ERROR}});
}

TEST_CASE("Command parser, construction") {
    BasicEnvironmentStub e;
    CommandParser p(e);
}

TEST_CASE("Command parser, simple command parser") {
    BasicEnvironmentStub e;
    e.appendSource(L("A"));

    CommandParser p(e);
    auto command = p.parse();
    checkCommandStub(command.get());
}

// TODO test compound commands and function definition command

TEST_CASE("Command parser, invalid symbol &") {
    auto message = L("unexpected symbol `&'; a command was expected");
    checkSyntaxError(L("& "), 0, Message<>(message));
}

TEST_CASE("Command parser, invalid symbol |") {
    auto message = L("unexpected symbol `|'; a command was expected");
    checkSyntaxError(L("| "), 0, Message<>(message));
}

TEST_CASE("Command parser, invalid symbol )") {
    auto message = L("unexpected symbol `)'; a command was expected");
    checkSyntaxError(L(")"), 0, Message<>(message));
}

TEST_CASE("Command parser, invalid symbol ;") {
    auto message = L("unexpected symbol `;'; a command was expected");
    checkSyntaxError(L("; "), 0, Message<>(message));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
