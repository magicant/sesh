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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironment.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/WordComponentParser.hh"
#include "language/parser/WordParserImpl.tcc"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::language::parser::BasicEnvironment;
using sesh::language::parser::Environment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::Predicate;
using sesh::language::parser::WordComponentParser;
using sesh::language::parser::WordParserImpl;
using sesh::language::source::Source;
using sesh::language::source::SourceStub;
using sesh::language::syntax::Printer;
using sesh::language::syntax::WordComponent;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

class EnvironmentStub : public BasicEnvironment {

    using BasicEnvironment::BasicEnvironment;

    bool mIsEof = false;

    bool isEof() const noexcept override {
        return mIsEof;
    }

    bool substituteAlias(const Iterator &, const Iterator &) override {
        throw "unexpected";
    }

    void addDiagnosticMessage(const Iterator &, String &&, ErrorLevel)
            override {
        throw "unexpected";
    }

public:

    void setIsEof(bool isEof = true) {
        mIsEof = isEof;
    }

    Iterator begin() const {
        return sourceBuffer().begin();
    }

    void appendSource(String &&s) {
        substituteSource([&s](Source::Pointer &&orig) -> Source::Pointer {
            auto length = (orig == nullptr) ? 0 : orig->length();
            return Source::Pointer(new SourceStub(
                    std::move(orig), length, length, std::move(s)));
        });
    }

    void checkSource(const String &string) {
        for (String::size_type i = 0; i < string.length(); ++i)
            CHECK(sourceBuffer().at(i) == string.at(i));
        CHECK_THROWS_AS(sourceBuffer().at(string.length()), std::out_of_range);
    }

};

class RawStringStub : public WordComponent {

    void print(Printer &) const override { throw "unexpected print"; }

};

class RawStringParserStub : public WordComponentParser, protected Parser {

public:

    RawStringParserStub(
            Environment &e,
            Predicate<Char> &&isDelimiter,
            bool removeLineContinuations = true)
            noexcept :
            WordComponentParser(),
            Parser(e) {
        CHECK((isDelimiter != nullptr));
        CHECK(removeLineContinuations);
    }

    std::unique_ptr<WordComponent> parse() override {
        if (environment().end() - environment().current() < 2)
            throw NeedMoreSource();
        environment().current() += 2;
        return std::unique_ptr<WordComponent>(new RawStringStub);
    }

};

class TestTypes {
public:
    using RawStringParser = RawStringParserStub;
};
using WordParser = WordParserImpl<TestTypes>;

template<Char c>
bool is(Environment &, Char c2) {
    return c == c2;
}

bool fail(Environment &, Char) {
    FAIL("unexpected predicate test");
    return true;
}

TEST_CASE("Word parser construction") {
    EnvironmentStub e;
    WordParser p(e, [](Environment &, Char) { return false; });
    WordParser(std::move(p));
}

TEST_CASE("Word parser empty word") {
    EnvironmentStub e;
    e.setIsEof();

    WordParser p(e, fail);
    auto result = p.parse();

    REQUIRE((result != nullptr));
    CHECK(result->components().empty());
    CHECK(e.current() == e.end());
}

TEST_CASE("Word parser raw string") {
    EnvironmentStub e;
    e.appendSource(L("AA!"));

    WordParser p(e, is<L('!')>);
    auto result = p.parse();

    REQUIRE((result != nullptr));
    CHECK(result->components().size() == 1);
    CHECK(e.current() == e.begin() + 2);
    RawStringStub *rss =
            dynamic_cast<RawStringStub *>(result->components()[0].get());
    CHECK((rss != nullptr));
}

TEST_CASE("Word parser re-parse") {
    EnvironmentStub e;
    WordParser p(e, is<L('X')>);

    e.appendSource(L("!"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("!"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("X"));
    auto result = p.parse();
    REQUIRE((result != nullptr));
    CHECK(result->components().size() == 1);
    CHECK(e.current() == e.begin() + 2);
    RawStringStub *rss =
            dynamic_cast<RawStringStub *>(result->components()[0].get());
    CHECK((rss != nullptr));
}

TEST_CASE("Word parser line continuation") {
    EnvironmentStub e;
    e.appendSource(L("\\\nAA\\\n!"));

    WordParser p(e, is<L('!')>);
    auto result = p.parse();

    e.checkSource(L("AA!"));
    REQUIRE((result != nullptr));
    CHECK(result->components().size() == 1);
    CHECK(e.current() == e.begin() + 2);
    RawStringStub *rss =
            dynamic_cast<RawStringStub *>(result->components()[0].get());
    CHECK((rss != nullptr));
}

// TODO other word component parser types

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
