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
#include "common/Char.hh"
#include "language/parser/AndOrListParser.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/Converter.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/Pipeline.hh"

namespace sesh {
namespace language {
namespace parser {

using sesh::common::Char;
using sesh::language::parser::CharParser;
using sesh::language::parser::Converter;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::Parser;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::syntax::Pipeline;

using Condition = sesh::language::syntax::ConditionalPipeline::Condition;
using Synchronicity = sesh::language::syntax::AndOrList::Synchronicity;

class AndOrListParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

bool isP(const Environment &, Char c) noexcept {
    return c == L('P');
}

class PipelineParserStub :
        public Converter<CharParser, std::unique_ptr<Pipeline>> {

public:

    explicit PipelineParserStub(Environment &e) :
            Converter(e, e, isP) { }

private:

    void convert(Char &&) override {
        result().emplace(new Pipeline);
    }

}; // class PipelineParserStub

std::unique_ptr<Parser<std::unique_ptr<Pipeline>>> newPipelineParser(
        Environment &e) {
    return std::unique_ptr<Parser<std::unique_ptr<Pipeline>>>(
            new PipelineParserStub(e));
}

TEST_CASE("And-or list parser, construction") {
    AndOrListParserTestEnvironment e;
    AndOrListParser p(newPipelineParser(e));
    (void) p;
}

TEST_CASE("And-or list parser, failure in first pipeline") {
    AndOrListParserTestEnvironment e;
    AndOrListParser p(newPipelineParser(e));

    e.appendSource(L("X"));
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("And-or list parser, 1 pipeline followed by nothing") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P"));
    e.setIsEof();

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 1);
}

TEST_CASE("And-or list parser, 1 pipeline followed by semicolon") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P; "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 2);
}

TEST_CASE("And-or list parser, 1 pipeline followed by ampersand") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P& "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::ASYNCHRONOUS);
    CHECK(e.position() == 2);
}

TEST_CASE("And-or list parser, 1 pipeline followed by double semicolon") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P;; "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 1);
}

TEST_CASE("And-or list parser, 1 pipeline followed by double ampersand") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P&& "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 1);
}

TEST_CASE("And-or list parser, 3 pipelines followed by nothing") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P&&P||P"));
    e.setIsEof();

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 2);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 7);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::OR_ELSE);
}

TEST_CASE("And-or list parser, 3 pipelines followed by semicolon") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P&&P||P; "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 2);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 8);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::OR_ELSE);
}

TEST_CASE("And-or list parser, 3 pipelines followed by ampersand") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P&&P||P& "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 2);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::ASYNCHRONOUS);
    CHECK(e.position() == 8);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::OR_ELSE);
}

TEST_CASE("And-or list parser, 3 pipelines followed by double semicolon") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P||P&&P;; "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 2);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 7);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::OR_ELSE);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::AND_THEN);
}

TEST_CASE("And-or list parser, 6 pipelines followed by double ampersand") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P||P||P&&P&&P||P&& "));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 5);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 16);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::OR_ELSE);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::OR_ELSE);
    CHECK(p.parse().value()->rest().at(2).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(2).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(4).condition() == Condition::OR_ELSE);
}

TEST_CASE("And-or list parser, reset") {
    AndOrListParserTestEnvironment e;
    e.appendSource(L("P;P||P&X"));

    AndOrListParser p(newPipelineParser(e));
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().empty());
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 2);

    p.reset();
    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 1);
    CHECK(p.parse().value()->rest().at(0).condition() == Condition::OR_ELSE);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::ASYNCHRONOUS);
    CHECK(e.position() == 7);

    p.reset();
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("And-or list parser, incomplete parse") {
    AndOrListParserTestEnvironment e;
    AndOrListParser p(newPipelineParser(e));

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("P"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("&"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("&"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("P"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("|"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("|"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L("P"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.appendSource(L(";"));
    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    e.setIsEof();

    REQUIRE(p.parse().hasValue());
    REQUIRE(p.parse().value() != nullptr);
    CHECK(p.parse().value()->rest().size() == 2);
    CHECK(p.parse().value()->synchronicity() == Synchronicity::SEQUENTIAL);
    CHECK(e.position() == 8);

    CHECK(p.parse().value()->rest().at(0).condition() == Condition::AND_THEN);
    CHECK(p.parse().value()->rest().at(1).condition() == Condition::OR_ELSE);
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
