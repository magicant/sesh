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

#include <initializer_list>
#include <memory>
#include <utility>
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/PipelineParser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::CharTraits;
using sesh::common::String;
using sesh::language::parser::CLocaleEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::PipelineParser;
using sesh::language::parser::StringParser;
using sesh::language::parser::isTokenDelimiter;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;

class CommandStub : public Command {

private:

    String mValue;

    void print(Printer &) const override { throw "unexpected print"; }

public:

    explicit CommandStub(String &&s) : mValue(std::move(s)) { }

    const String &value() const { return mValue; }

};

class CommandParserStub : public Parser<std::unique_ptr<Command>> {

private:

    StringParser mInnerParser;

public:

    explicit CommandParserStub(Environment &e) :
            mInnerParser(e, isTokenDelimiter) { }

    std::unique_ptr<Command> parse() {
        String s = mInnerParser.parse();
        if (s.empty())
            return nullptr;
        return std::unique_ptr<Command>(new CommandStub(std::move(s)));
    }

};

std::unique_ptr<Parser<std::unique_ptr<Command>>>
createCommandParser(Environment &e) {
    auto c = dereference(e, e.current());
    if (CharTraits::eq_int_type(c, CharTraits::eof()))
        return nullptr;
    return std::unique_ptr<Parser<std::unique_ptr<Command>>>(
            new CommandParserStub(e));
}

void checkPipelineCommand(
        const Pipeline &p,
        unsigned index,
        const String &value) {
    const CommandStub *c =
            dynamic_cast<CommandStub *>(p.commands().at(index).get());
    REQUIRE(c != nullptr);
    CHECK(c->value() == value);
}

void checkPipelineCommands(
        const Pipeline &p, std::initializer_list<String> values) {
    CHECK(p.commands().size() == values.size());

    unsigned i = 0;
    for (const String &value : values) {
        checkPipelineCommand(p, i, value);
        ++i;
    }
}

void checkSyntaxError(String &&source) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(source));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(pl == nullptr);
    // TODO check syntax error
}

#define CHECK_SYNTAX_ERROR(source) \
        do { INFO(source); checkSyntaxError(L(source)); } while (0)

TEST_CASE("Pipeline parser, construction") {
    CLocaleEnvironmentStub e;
    PipelineParser p(e, createCommandParser);
    PipelineParser(std::move(p));
}

TEST_CASE("Pipeline parser, empty input") {
    CHECK_SYNTAX_ERROR("");
    CHECK_SYNTAX_ERROR("\n");
}

TEST_CASE("Pipeline parser, single command without negation") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("command"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("command")});
}

TEST_CASE("Pipeline parser, single command with negation") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("! command"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    checkPipelineCommands(*pl, {L("command")});
}

TEST_CASE("Pipeline parser, triple command without negation") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("one| two|  three"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("one"), L("two"), L("three")});
}

TEST_CASE("Pipeline parser, triple command with negation") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("! one| two|  three"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    checkPipelineCommands(*pl, {L("one"), L("two"), L("three")});
}

TEST_CASE("Pipeline parser, line continuation and tab after '!'") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("! \\\n\\\n\t command"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    checkPipelineCommands(*pl, {L("command")});
}

TEST_CASE("Pipeline parser, line continuation and tab after '|'") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("one| \\\n\t two|  \\\n\\\nthree"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("one"), L("two"), L("three")});
}

TEST_CASE("Pipeline parser, newline after '|'") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("one| \n two|  \n\nthree"));
    e.setIsEof();

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end());
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("one"), L("two"), L("three")});
}

TEST_CASE("Pipeline parser, delimiter after single command") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("command||"));

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end() - 2);
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("command")});
}

TEST_CASE("Pipeline parser, delimiter after double command") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("one| two||"));

    PipelineParser p(e, createCommandParser);
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end() - 2);
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    checkPipelineCommands(*pl, {L("one"), L("two")});
}

TEST_CASE("Pipeline parser, need more source") {
    CLocaleEnvironmentStub e;
    PipelineParser p(e, createCommandParser);
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("!"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L(" "));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\t"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\\\n"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("one"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("|"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L(" "));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\t"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\n"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\\\n"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("two"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("|"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("three"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("|"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("|"));
    std::unique_ptr<Pipeline> pl = p.parse();
    CHECK(e.current() == e.end() - 2);
    REQUIRE(pl != nullptr);
    CHECK(pl->exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    checkPipelineCommands(*pl, {L("one"), L("two"), L("three")});
}

// TODO alias substitution in first command without '!'
// TODO alias substitution in first command after '!'
// TODO alias substitution in second command

TEST_CASE("Pipeline parser, invalid '!' after '!'") {
    CHECK_SYNTAX_ERROR("! !");
}

TEST_CASE("Pipeline parser, invalid '!' after '|'") {
    CHECK_SYNTAX_ERROR("one| !");
}

TEST_CASE("Pipeline parser, missing command after '!'") {
    CHECK_SYNTAX_ERROR("!");
    CHECK_SYNTAX_ERROR("! # comment");
}

TEST_CASE("Pipeline parser, missing command after '|'") {
    CHECK_SYNTAX_ERROR("one|");
    CHECK_SYNTAX_ERROR("one| # comment");
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
