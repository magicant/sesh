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
#include <vector>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/LineMode.hh"
#include "language/parser/Parser.hh"
#include "language/parser/SequenceParser.hh"
#include "language/parser/StringParser.hh"
#include "language/parser/token.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/Sequence.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::CLocaleEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::LineMode;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::SequenceParser;
using sesh::language::parser::StringParser;
using sesh::language::parser::isTokenDelimiter;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;
using sesh::language::syntax::Sequence;

using AndOrListParserPointer = SequenceParser::AndOrListParserPointer;

class CommandStub : public Command {

private:

    String mValue;

    void print(Printer &) const override { throw "unexpected print"; }

public:

    explicit CommandStub(String &&s) : mValue(std::move(s)) { }

    const String &value() const { return mValue; }

};

class AndOrListParserStub : public Parser<std::unique_ptr<AndOrList>> {

private:

    StringParser mInnerParser;

public:

    explicit AndOrListParserStub(Environment &e) :
            mInnerParser(e, isTokenDelimiter) { }

    std::unique_ptr<AndOrList> parse() override {
        String s = mInnerParser.parse();
        if (s.empty())
            return nullptr;
        Pipeline p;
        p.commands().emplace_back(new CommandStub(std::move(s)));
        return std::unique_ptr<AndOrList>(new AndOrList(std::move(p)));
    }

}; // class AndOrListParserStub

AndOrListParserPointer createAndOrListParser(Environment &e) {
    return AndOrListParserPointer(new AndOrListParserStub(e));
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

void checkAndOrList(const AndOrList *l, const String &value) {
    CHECK(l != nullptr);
    if (l == nullptr)
        return;

    CHECK(l->synchronicity() == AndOrList::Synchronicity::SEQUENTIAL);
    CHECK(l->rest().empty());
    checkPipeline(&l->first(), value);
}

void checkSequence(const Sequence *s, std::initializer_list<String> values) {
    CHECK(s != nullptr);
    if (s == nullptr)
        return;

    CHECK(s->andOrLists().size() == values.size());

    unsigned i = 0;
    for (auto &value : values) {
        if (i < s->andOrLists().size())
            checkAndOrList(s->andOrLists()[i].get(), value);
        ++i;
    }
}

void checkEmptyResult(String &&source, LineMode lm, String::size_type length) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(source));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, lm);
    std::unique_ptr<Sequence> s = p.parse();
    CHECK(e.current() == e.begin() + length);
    checkSequence(s.get(), {});
}

#define CHECK_EMPTY_RESULT(source, lineMode, length) \
        do { \
            INFO(source << ' ' << (unsigned) lineMode); \
            checkEmptyResult(L(source), lineMode, length); \
        } while (0)

#define CHECK_EMPTY_RESULT_ALL_LINE_MODES(source, length) \
        do { \
            CHECK_EMPTY_RESULT(source, LineMode::SINGLE_LINE, length); \
            CHECK_EMPTY_RESULT(source, LineMode::MULTI_LINE, length); \
        } while (0)

void checkSyntaxError(String &&source, LineMode lm) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(source));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, lm);
    auto s = p.parse();
    CHECK(s == nullptr);
}

#define CHECK_SYNTAX_ERROR(source, lineMode) \
        do { INFO(source); checkSyntaxError(L(source), lineMode); } while (0)

TEST_CASE("Sequence parser, construction") {
    BasicEnvironmentStub e;
    SequenceParser(e, createAndOrListParser, LineMode::SINGLE_LINE);
    SequenceParser(e, createAndOrListParser, LineMode::MULTI_LINE);
}

TEST_CASE("Sequence parser, empty results") {
    CHECK_EMPTY_RESULT("\n", LineMode::SINGLE_LINE, 1);
    CHECK_EMPTY_RESULT(" \n", LineMode::SINGLE_LINE, 2);
    CHECK_EMPTY_RESULT(" \\\n\n", LineMode::SINGLE_LINE, 2);

    CHECK_EMPTY_RESULT(" \t\n\\\n\t\n ", LineMode::MULTI_LINE, 6);

    CHECK_EMPTY_RESULT_ALL_LINE_MODES("", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES(" ", 1);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES(")", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES(";;", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("}", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("do", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("done", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("elif", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("else", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("esac", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("fi", 0);
    CHECK_EMPTY_RESULT_ALL_LINE_MODES("then", 0);

    CHECK_EMPTY_RESULT_ALL_LINE_MODES("\tth\\\nen\\\n\n", 1);
}

TEST_CASE("Sequence parser, single and-or list, single line, eof") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("list"));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end());
    checkSequence(s.get(), {L("list")});
}

TEST_CASE("Sequence parser, single and-or list, single line, newline") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("list\n "));

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkSequence(s.get(), {L("list")});
}

TEST_CASE("Sequence parser, single and-or list, single line, stop word") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("test)"));

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkSequence(s.get(), {L("test")});
}

TEST_CASE("Sequence parser, triple and-or list, single line, eof") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("apple banana cherry"));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end());
    checkSequence(s.get(), {L("apple"), L("banana"), L("cherry")});
}

TEST_CASE("Sequence parser, triple and-or list, newline") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("apple banana cherry\n\n"));

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end() - 1);
    checkSequence(s.get(), {L("apple"), L("banana"), L("cherry")});
}

TEST_CASE("Sequence parser, triple and-or list, single line, stop word") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("apple banana\tcherry done"));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, LineMode::SINGLE_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end() - 4);
    checkSequence(s.get(), {L("apple"), L("banana"), L("cherry")});
}

TEST_CASE("Sequence parser, double and-or list, multi-line, eof") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("\none \n\t two"));
    e.setIsEof();

    SequenceParser p(e, createAndOrListParser, LineMode::MULTI_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end());
    checkSequence(s.get(), {L("one"), L("two")});
}

TEST_CASE("Sequence parser, and-or list, multi-line, stop word") {
    CLocaleEnvironmentStub e;
    e.appendSource(L("\none \n\t two then)"));

    SequenceParser p(e, createAndOrListParser, LineMode::MULTI_LINE);
    auto s = p.parse();
    CHECK(e.current() == e.end() - 5);
    checkSequence(s.get(), {L("one"), L("two")});
}

TEST_CASE("Sequence parser, need more source") {
    CLocaleEnvironmentStub e;
    SequenceParser p(e, createAndOrListParser, LineMode::MULTI_LINE);

    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);
    for (Char c : String(L(" A\t\nB do"))) {
        e.appendSource(String(1, c));
        INFO(c);
        REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);
    }
    e.setIsEof();

    auto s = p.parse();
    CHECK(e.current() == e.end() - 2);
    checkSequence(s.get(), {L("A"), L("B")});
}

TEST_CASE("Sequence parser, and-or list parse error") {
    CHECK_SYNTAX_ERROR(";", LineMode::SINGLE_LINE);
    CHECK_SYNTAX_ERROR(";", LineMode::MULTI_LINE);
    CHECK_SYNTAX_ERROR("A ;", LineMode::SINGLE_LINE);
    CHECK_SYNTAX_ERROR("A ;", LineMode::MULTI_LINE);
}

// TODO alias substitution
// TODO pending redirections

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
