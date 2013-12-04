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
#include "common/Char.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/WordComponentParserBase.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::Char;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Parser;
using sesh::language::parser::ParserBase;
using sesh::language::parser::Predicate;
using sesh::language::parser::WordComponentParserBase;
using sesh::language::syntax::Printer;
using sesh::language::syntax::WordComponent;

class RawStringStub final : public WordComponent {

    void print(Printer &) const override { throw "unexpected print"; }

};

class RawStringParserStub final :
        public Parser<std::unique_ptr<WordComponent>>, protected ParserBase {

public:

    RawStringParserStub(Environment &e) noexcept : Parser(), ParserBase(e) { }

    std::unique_ptr<WordComponent> parse() override {
        if (environment().end() - environment().current() < 2)
            throw NeedMoreSource();
        environment().current() += 2;
        return std::unique_ptr<WordComponent>(new RawStringStub);
    }

};

class WordComponentParser final : public WordComponentParserBase {

    using WordComponentParserBase::WordComponentParserBase;

    ParserPointer createRawStringParser(
            Predicate<Char> &&isDelimiter,
            LineContinuationTreatment lct = LineContinuationTreatment::REMOVE)
            const override {
        CHECK(isDelimiter != nullptr);
        CHECK(lct == LineContinuationTreatment::REMOVE);
        return ParserPointer(new RawStringParserStub(environment()));
    }

}; // class WordComponentParser

template<bool result>
bool always(const Environment &, Char) {
    return result;
}

bool fail(const Environment &, Char) {
    FAIL("unexpected predicate test");
    return true;
}

TEST_CASE("Word component parser, construction") {
    BasicEnvironmentStub e;
    WordComponentParser p(e, fail);
}

TEST_CASE("Word component parser, empty word") {
    BasicEnvironmentStub e;
    e.setIsEof();

    WordComponentParser p(e, fail);
    CHECK(p.parse() == nullptr);
}

TEST_CASE("Word component parser, no component") {
    BasicEnvironmentStub e;
    e.appendSource(L("A"));

    WordComponentParser p(
            e,
            [&e](const Environment &e2, Char c) {
                CHECK(&e == &e2);
                CHECK(c == L('A'));
                return true;
            });

    CHECK(p.parse() == nullptr);
    CHECK(e.current() == e.begin());
    e.checkSource(L("A"));
}

TEST_CASE("Word component parser, raw string") {
    BasicEnvironmentStub e;
    e.appendSource(L("AA"));

    WordComponentParser p(e, always<false>);
    std::unique_ptr<WordComponent> wc = p.parse();

    e.checkSource(L("AA"));
    CHECK(e.current() == e.end());
    CHECK(wc != nullptr);
    CHECK(dynamic_cast<RawStringStub *>(wc.get()) != nullptr);
}

TEST_CASE("Word component parser, raw string with line continuations") {
    BasicEnvironmentStub e;
    e.appendSource(L("\\\n\\\nAA"));

    WordComponentParser p(e, always<false>);
    std::unique_ptr<WordComponent> wc = p.parse();

    e.checkSource(L("AA"));
    CHECK(e.current() == e.end());
    CHECK(wc != nullptr);
    CHECK(dynamic_cast<RawStringStub *>(wc.get()) != nullptr);
}

TEST_CASE("Word component parser, need more source") {
    BasicEnvironmentStub e;
    WordComponentParser p(e, always<false>);
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\\"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\n"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\\"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("\n"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("1"));
    REQUIRE_THROWS_AS(p.parse(), NeedMoreSource);

    e.appendSource(L("2"));
    std::unique_ptr<WordComponent> wc = p.parse();

    e.checkSource(L("12"));
    CHECK(e.current() == e.end());
    CHECK(wc != nullptr);
    CHECK(dynamic_cast<RawStringStub *>(wc.get()) != nullptr);
}

// TODO other word component parser types

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
