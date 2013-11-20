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
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/CommandParserBase.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::CommandParserBase;
using sesh::language::parser::Parser;
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

// TODO test reuse

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
