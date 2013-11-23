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
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/AndOrListParser.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::AndOrListParser;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::CLocaleEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::StringParser;
using sesh::language::parser::isTokenDelimiter;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Command;
using sesh::language::syntax::ConditionalPipeline;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;

using Condition = ConditionalPipeline::Condition;
using PipelineParserPointer = AndOrListParser::PipelineParserPointer;
using Synchronicity = AndOrList::Synchronicity;

class CommandStub : public Command {

private:

    String mValue;

    void print(Printer &) const override { throw "unexpected print"; }

public:

    explicit CommandStub(String &&s) : mValue(std::move(s)) { }

    const String &value() const { return mValue; }

};

class PipelineParserStub : public Parser<std::unique_ptr<Pipeline>> {

private:

    StringParser mInnerParser;

public:

    explicit PipelineParserStub(Environment &e) :
            mInnerParser(e, isTokenDelimiter) { }

    std::unique_ptr<Pipeline> parse() override {
        String s = mInnerParser.parse();
        if (s.empty())
            return nullptr;
        auto p = std::unique_ptr<Pipeline>(new Pipeline);
        p->commands().emplace_back(new CommandStub(std::move(s)));
        return std::move(p);
    }

};

PipelineParserPointer createPipelineParser(Environment &e) {
    return PipelineParserPointer(new PipelineParserStub(e));
}

void checkPipeline(const Pipeline *p, const String &value) {
    CHECK(p != nullptr);
    if (p == nullptr)
        return;

    CHECK(p->commands().size() == 1);
    if (p->commands().empty())
        return;

    const CommandStub *c = dynamic_cast<CommandStub *>(p->commands()[0].get());
    CHECK(c != nullptr);
    if (c != nullptr)
        CHECK(c->value() == value);
}

void checkConditionalPipeline(
        const ConditionalPipeline &p, const std::pair<Condition, String> &cs) {
    CHECK(p.condition() == cs.first);
    checkPipeline(&p.pipeline(), cs.second);
}

void checkAndOrList(
        const AndOrList *l,
        const String &first,
        std::initializer_list<std::pair<Condition, String>> rest,
        Synchronicity s) {
    CHECK(l != nullptr);
    if (l == nullptr)
        return;

    CHECK(l->synchronicity() == s);
    checkPipeline(&l->first(), first);
    CHECK(l->rest().size() == rest.size());

    unsigned i = 0;
    for (auto &r : rest) {
        if (i < l->rest().size())
            checkConditionalPipeline(l->rest()[i], r);
        ++i;
    }
}

void checkSyntaxError(String &&source) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(source));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(l == nullptr);
    // TODO check syntax error
}

#define CHECK_SYNTAX_ERROR(source) \
        do { INFO(source); checkSyntaxError(L(source)); } while (0)

TEST_CASE("And-or list parser, construction") {
    BasicEnvironmentStub e;
    AndOrListParser(e, createPipelineParser);
}

TEST_CASE("And-or list parser, empty input") {
    CHECK_SYNTAX_ERROR("");
    CHECK_SYNTAX_ERROR("\n");
}

TEST_CASE("And-or list parser, single pipeline") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A"));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(l.get(), L("A"), {}, Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, and-then") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A&& B"));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(
            l.get(),
            L("A"),
            {{Condition::AND_THEN, L("B")}},
            Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, or-else") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A|| B"));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(
            l.get(),
            L("A"),
            {{Condition::OR_ELSE, L("B")}},
            Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, comment, newline, and line continuation") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A\\\n|\\\n| # hello\n\n\\\n B"));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(
            l.get(),
            L("A"),
            {{Condition::OR_ELSE, L("B")}},
            Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, many pipelines") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A&& B|| C||D&& E"));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(
            l.get(),
            L("A"),
            {{Condition::AND_THEN, L("B")},
             {Condition::OR_ELSE, L("C")},
             {Condition::OR_ELSE, L("D")},
             {Condition::AND_THEN, L("E")}},
            Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, single pipeline followed by semicolon") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A; "));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkAndOrList(l.get(), L("A"), {}, Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, single pipeline followed by ampersand") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A& "));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkAndOrList(l.get(), L("A"), {}, Synchronicity::ASYNCHRONOUS);
}

TEST_CASE("And-or list parser, single pipeline followed by double semicolon") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("A;; "));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.begin() + 1);
    checkAndOrList(l.get(), L("A"), {}, Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, many pipelines followed by ampersand") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("X||Y&&Z& "));
    e.setIsEof();

    AndOrListParser p(e, createPipelineParser);
    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkAndOrList(
            l.get(),
            L("X"),
            {{Condition::OR_ELSE, L("Y")}, {Condition::AND_THEN, L("Z")}},
            Synchronicity::ASYNCHRONOUS);
}

TEST_CASE("And-or list parser, need more source") {
    CLocaleEnvironmentStub e;
    AndOrListParser p(e, createPipelineParser);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);
    for (Char c : String(L("A&& B|| C;"))) {
        e.appendSource(String(1, c));
        INFO(c);
        REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);
    }
    e.setIsEof();

    std::unique_ptr<AndOrList> l = p.parse();
    CHECK(e.current() == e.end());
    checkAndOrList(
            l.get(),
            L("A"),
            {{Condition::AND_THEN, L("B")}, {Condition::OR_ELSE, L("C")}},
            Synchronicity::SEQUENTIAL);
}

TEST_CASE("And-or list parser, pipeline parse error") {
    CHECK_SYNTAX_ERROR("");
    CHECK_SYNTAX_ERROR("A&& ");
    CHECK_SYNTAX_ERROR("A&&B||)");
}

// TODO test alias substitution

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
