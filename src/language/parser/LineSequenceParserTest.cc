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
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/DiagnosticEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/LineSequenceParser.hh"
#include "language/parser/Parser.hh"
#include "language/parser/SequenceParserResult.hh"

namespace {

using sesh::common::Char;
using sesh::common::ErrorLevel;
using sesh::common::Message;
using sesh::common::String;
using sesh::language::parser::CharParser;
using sesh::language::parser::DiagnosticTestEnvironment;
using sesh::language::parser::Environment;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::Keyword;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::LineSequenceParser;
using sesh::language::parser::Parser;
using sesh::language::parser::SequenceParserResult;
using sesh::language::parser::SourceTestEnvironment;

using NewlineParserPointer = std::unique_ptr<Parser<Char>>;
using SequenceParserPointer = std::unique_ptr<Parser<SequenceParserResult>>;

class LineSequenceParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment,
        public DiagnosticTestEnvironment {
};

class SequenceParserStub : public Parser<SequenceParserResult> {

private:

    CharParser mCharParser;
    SequenceParserResult mResult;

    constexpr static bool isKOrS(const Environment &, Char c) noexcept {
        return c == L('K') || c == L('S');
    }

public:

    explicit SequenceParserStub(Environment &e) :
            Parser(e), mCharParser(e, isKOrS), mResult() { }

private:

    void parseImpl() override {
        result() = &mResult;

        Char *c = mCharParser.parse();
        if (c != nullptr && *c == L('K'))
            mResult.second.emplace(Keyword::keywordFi(), mCharParser.begin());
    }

    void resetImpl() noexcept override {
        mCharParser.reset();
        mResult.second.clear();
        Parser::resetImpl();
    }

}; // class SequenceParserStub

constexpr bool isNewline(const Environment &, Char c) noexcept {
    return c == L('\n');
}

LineSequenceParser createLineSequenceParser(Environment &e) {
    return LineSequenceParser(
            SequenceParserPointer(new SequenceParserStub(e)),
            NewlineParserPointer(new CharParser(e, isNewline)));
}

TEST_CASE("Line sequence parser, construction and assignment") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);
    p = LineSequenceParser(std::move(p));
}

TEST_CASE("Line sequence parser, empty sequence with newline") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L("\n\n"));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->andOrLists().empty());
    CHECK(e.position() == 1);
    CHECK(e.diagnosticMessages().empty());
}

TEST_CASE("Line sequence parser, empty sequence at end-of-file") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.setIsEof();
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->andOrLists().empty());
    CHECK(e.position() == 0);
    CHECK(e.diagnosticMessages().empty());
}

TEST_CASE("Line sequence parser, error for closing token") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L("K\n"));
    REQUIRE(p.parse() != nullptr);
    CHECK(p.parse()->andOrLists().empty());
    CHECK(e.position() == e.length());

    Message<> m(L("encountered `fi' without a matching `if'"));
    e.checkMessages({{0, m, ErrorLevel::ERROR}});
}

TEST_CASE("Line sequence parser, error for trailing operator") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L(") "));
    CHECK(p.parse() != nullptr);

    Message<> m(L("encountered `)' without a matching `('"));
    e.checkMessages({{0, m, ErrorLevel::ERROR}});
}

TEST_CASE("Line sequence parser, error for null character") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(String(L("\0"), 1));
    CHECK(p.parse() != nullptr);

    Message<> m(L("unexpected null character"));
    e.checkMessages({{0, m, ErrorLevel::ERROR}});
}

TEST_CASE("Line sequence parser, error for carriage return") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L("\r"));
    CHECK(p.parse() != nullptr);

    Message<> m(L("unexpected carriage return"));
    e.checkMessages({{0, m, ErrorLevel::ERROR}});
}

TEST_CASE("Line sequence parser, error for invalid trailing character") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L("\a"));
    CHECK(p.parse() != nullptr);

    Message<> m(L("unrecognized character `\a'"));
    e.checkMessages({{0, m, ErrorLevel::ERROR}});
}

TEST_CASE("Line sequence parser, reset") {
    LineSequenceParserTestEnvironment e;
    LineSequenceParser p = createLineSequenceParser(e);

    e.appendSource(L("\n\n"));
    CHECK(p.parse() != nullptr);
    CHECK(e.position() == 1);
    CHECK(e.diagnosticMessages().empty());

    p.reset();
    CHECK(p.parse() != nullptr);
    CHECK(e.position() == 2);
    CHECK(e.diagnosticMessages().empty());

    e.appendSource(L("K\n"));
    p.reset();
    CHECK(p.parse() != nullptr);
    CHECK(e.diagnosticMessages().size() == 1);

    e.setPosition(e.length());
    e.appendSource(L(")\n"));
    p.reset();
    CHECK(p.parse() != nullptr);
    CHECK(e.diagnosticMessages().size() == 2);

    e.setPosition(e.length());
    e.appendSource(L("\r\n"));
    p.reset();
    CHECK(p.parse() != nullptr);
    CHECK(e.diagnosticMessages().size() == 3);

    e.setPosition(e.length());
    e.setIsEof();
    p.reset();
    CHECK(p.parse() != nullptr);
    CHECK(e.position() == e.length());
    CHECK(e.diagnosticMessages().size() == 3);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
