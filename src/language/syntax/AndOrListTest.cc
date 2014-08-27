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
#include <stdexcept>
#include "common/String.hh"
#include "common/xchar.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::String;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Command;
using sesh::language::syntax::ConditionalPipeline;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;

struct CommandStub : public Command {
    using Command::Command;
    void print(Printer &p) const override {
        p << L("command");
        p.delayedCharacters() << L(' ');
    }
}; // struct CommandStub

Pipeline pipelineStub() {
    Pipeline p(Pipeline::ExitStatusType::NEGATED);
    p.commands().push_back(Pipeline::CommandPointer(new CommandStub));
    return p;
}

void testAndOrListWithoutRest(
        Printer::LineMode lineMode,
        AndOrList::Synchronicity synchronicity,
        const String withoutDelayed,
        const String withDelayed) {
    AndOrList aol(pipelineStub(), synchronicity);
    Printer p(lineMode);

    p.indentLevel() = 1;
    p.delayedCharacters() << L("X");
    p.delayedLines() << L("Y\n");

    p << aol;
    CHECK(p.toString() == withoutDelayed);

    p << L("");
    CHECK(p.toString() == withDelayed);
}

void testAndOrListWithRest(
        Printer::LineMode lineMode,
        AndOrList::Synchronicity synchronicity,
        const String withoutDelayed,
        const String withDelayed) {
    AndOrList aol(pipelineStub(), synchronicity);
    aol.rest().emplace_back(
            ConditionalPipeline::Condition::AND_THEN,
            ConditionalPipeline::PipelinePointer(
                    new Pipeline(pipelineStub())));
    aol.rest().emplace_back(
            ConditionalPipeline::Condition::OR_ELSE,
            ConditionalPipeline::PipelinePointer(
                    new Pipeline(pipelineStub())));

    Printer p(lineMode);
    p.indentLevel() = 1;
    p.delayedCharacters() << L("X");
    p.delayedLines() << L("Y\n");

    p << aol;
    CHECK(p.toString() == withoutDelayed);

    p << L("");
    CHECK(p.toString() == withDelayed);
}

TEST_CASE("And-or list constructor 1") {
    AndOrList aol(pipelineStub());
    aol.rest().emplace_back(ConditionalPipeline::Condition::AND_THEN);
    aol.rest()[0].pipeline().commands().push_back(
            Pipeline::CommandPointer(new CommandStub));
    aol.rest()[0].pipeline().commands().push_back(
            Pipeline::CommandPointer(new CommandStub));

    CHECK(aol.synchronicity() == AndOrList::Synchronicity::SEQUENTIAL);
    CHECK(aol.first().exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    CHECK(aol.first().commands().size() == 1);
    CHECK_NOTHROW((void)
            dynamic_cast<CommandStub &>(*aol.first().commands()[0]));
    CHECK(aol.rest().size() == 1);

    const Pipeline &p1 = aol.rest()[0].pipeline();
    CHECK(p1.commands().size() == 2);
    CHECK_NOTHROW((void) dynamic_cast<CommandStub &>(*p1.commands()[0]));
    CHECK_NOTHROW((void) dynamic_cast<CommandStub &>(*p1.commands()[1]));
}

TEST_CASE("And-or list constructor 2") {
    AndOrList aol(
            Pipeline(Pipeline::ExitStatusType::NEGATED),
            AndOrList::Synchronicity::ASYNCHRONOUS);

    CHECK(aol.first().commands().empty());
    CHECK(aol.first().exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    CHECK(aol.rest().empty());
    CHECK(aol.synchronicity() == AndOrList::Synchronicity::ASYNCHRONOUS);
}

TEST_CASE("And-or list print w/o rest") {
    testAndOrListWithoutRest(
            Printer::LineMode::SINGLE_LINE,
            AndOrList::Synchronicity::SEQUENTIAL,
            L("X! command"),
            L("X! command; "));
    testAndOrListWithoutRest(
            Printer::LineMode::MULTI_LINE,
            AndOrList::Synchronicity::SEQUENTIAL,
            L("X! command"),
            L("X! command; "));
    testAndOrListWithoutRest(
            Printer::LineMode::SINGLE_LINE,
            AndOrList::Synchronicity::ASYNCHRONOUS,
            L("X! command&"),
            L("X! command& "));
    testAndOrListWithoutRest(
            Printer::LineMode::MULTI_LINE,
            AndOrList::Synchronicity::ASYNCHRONOUS,
            L("X! command&"),
            L("X! command& "));
}

TEST_CASE("And-or list print w/ rest") {
    testAndOrListWithRest(
            Printer::LineMode::SINGLE_LINE,
            AndOrList::Synchronicity::SEQUENTIAL,
            L("X! command && ! command || ! command"),
            L("X! command && ! command || ! command; "));
    testAndOrListWithRest(
            Printer::LineMode::MULTI_LINE,
            AndOrList::Synchronicity::SEQUENTIAL,
            L("X! command &&\nY\n    ! command ||\n    ! command"),
            L("X! command &&\nY\n    ! command ||\n    ! command; "));
    testAndOrListWithRest(
            Printer::LineMode::SINGLE_LINE,
            AndOrList::Synchronicity::ASYNCHRONOUS,
            L("X! command && ! command || ! command&"),
            L("X! command && ! command || ! command& "));
    testAndOrListWithRest(
            Printer::LineMode::MULTI_LINE,
            AndOrList::Synchronicity::ASYNCHRONOUS,
            L("X! command &&\nY\n    ! command ||\n    ! command&"),
            L("X! command &&\nY\n    ! command ||\n    ! command& "));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
