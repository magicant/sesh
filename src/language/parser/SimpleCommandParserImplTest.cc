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
#include "common/String.hh"
#include "common/Variant.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/SimpleCommandParserImpl.hh"
#include "language/parser/SimpleCommandParserImpl.tcc"
#include "language/parser/WordParserTestHelper.hh"
#include "language/source/Source.hh"
#include "language/source/SourceTestHelper.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::common::Variant;
using sesh::language::parser::CLocaleEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::SimpleCommandParserImpl;
using sesh::language::parser::WordParserStub;
using sesh::language::parser::isTokenDelimiter;
using sesh::language::source::Source;
using sesh::language::source::SourceStub;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::RawString;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;

class AssignmentParserStub {

private:

    WordParserStub mWordParser;

public:

    AssignmentParserStub(Environment &e) : mWordParser(e, isTokenDelimiter) { }

    using AssignmentPointer = std::unique_ptr<Assignment>;
    using WordPointer = std::unique_ptr<Word>;
    using Result = Variant<AssignmentPointer, WordPointer>;

    Result parse() {
        auto word = mWordParser.parse();
        if (word->components().empty())
            return Result::of(WordPointer(std::move(word)));

        RawString *rs = dynamic_cast<RawString *>(word->components()[0].get());
        if (rs == nullptr)
            return Result::of(WordPointer(std::move(word)));

        auto &value = rs->value();
        auto i = value.find(L('='));
        if (i == String::npos)
            return Result::of(WordPointer(std::move(word)));

        auto name = value.substr(0, i);
        value.erase(0, i + 1);
        return Result::of(AssignmentPointer(
                    new Assignment(std::move(name), std::move(word))));
    }

};

class TestTypes {
public:
    using AssignmentParser = AssignmentParserStub;
    using WordParser = WordParserStub;
};
using SimpleCommandParser = SimpleCommandParserImpl<TestTypes>;

void checkWord(const Word *w, const String &value) {
    REQUIRE(w != nullptr);
    if (value.empty()) {
        CHECK(w->components().empty());
        return;
    }
    CHECK(w->components().size() == 1);
    RawString *rs = dynamic_cast<RawString *>(w->components().at(0).get());
    REQUIRE(rs != nullptr);
    CHECK(rs->value() == value);
}

void checkAssignment(
        const Assignment *a, const String &name, const String &value) {
    REQUIRE(a != nullptr);
    CHECK(a->variableName() == name);
    checkWord(&a->value(), value);
}

TEST_CASE("Simple command parser construction") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp1(e);
    SimpleCommandParser(std::move(scp1));
}

TEST_CASE("Simple command parser, empty command") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.setIsEof();

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end());
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().empty());
    CHECK(sc->words().empty());
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, empty command with comment") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L(" # Comment"));
    e.setIsEof();

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end());
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().empty());
    CHECK(sc->words().empty());
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, one word, immediate") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("test"));
    e.setIsEof();

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end());
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().empty());
    CHECK(sc->words().size() == 1);
    checkWord(sc->words().at(0).get(), L("test"));
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, one word, after space") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("   test;"));

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end() - 1);
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().empty());
    CHECK(sc->words().size() == 1);
    checkWord(sc->words().at(0).get(), L("test"));
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, one assignment, immediate") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("foo=bar\n"));

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end() - 1);
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().size() == 1);
    checkAssignment(sc->assignments().at(0).get(), L("foo"), L("bar"));
    CHECK(sc->words().empty());
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, one assignment, after space") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("\t\tfoo=bar"));
    e.setIsEof();

    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end());
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().size() == 1);
    checkAssignment(sc->assignments().at(0).get(), L("foo"), L("bar"));
    CHECK(sc->words().empty());
    // TODO CHECK(sc->redirections().empty());
}

TEST_CASE("Simple command parser, one redirection, immediate") {
    // TODO
}

TEST_CASE("Simple command parser, one redirection, after space") {
    // TODO
}

TEST_CASE("Simple command parser, assignments and words, eof") {
    CLocaleEnvironmentStub e;
    SimpleCommandParser scp(e);

    e.appendSource(L("a=A"));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("\t"));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("b=B"));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L(" "));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("1"));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L(" \tc=C\t "));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.appendSource(L("2"));
    REQUIRE_THROWS_AS(scp.parseSimpleCommand(), NeedMoreSource);

    e.setIsEof();
    std::unique_ptr<SimpleCommand> sc = scp.parseSimpleCommand();
    CHECK(e.current() == e.end());
    REQUIRE(sc != nullptr);
    CHECK(sc->assignments().size() == 2);
    checkAssignment(sc->assignments().at(0).get(), L("a"), L("A"));
    checkAssignment(sc->assignments().at(1).get(), L("b"), L("B"));
    CHECK(sc->words().size() == 3);
    checkWord(sc->words().at(0).get(), L("1"));
    checkWord(sc->words().at(1).get(), L("c=C"));
    checkWord(sc->words().at(2).get(), L("2"));
    // TODO CHECK(sc->redirections().empty());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
