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
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/command.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::and_or_list;
using sesh::language::syntax::command;
using sesh::language::syntax::ConditionalPipeline;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;

struct command_stub : public command {
    using command::command;
    void print(Printer &p) const override {
        p << L("command");
        p.delayedCharacters() << L(' ');
    }
}; // struct command_stub

Pipeline pipeline_stub() {
    Pipeline p(Pipeline::ExitStatusType::NEGATED);
    p.commands().push_back(Pipeline::CommandPointer(new command_stub));
    return p;
}

void test_and_or_list_without_rest(
        Printer::LineMode line_mode,
        and_or_list::synchronicity_type synchronicity,
        const xstring without_delayed,
        const xstring with_delayed) {
    and_or_list aol(pipeline_stub(), synchronicity);
    Printer p(line_mode);

    p.indentLevel() = 1;
    p.delayedCharacters() << L("X");
    p.delayedLines() << L("Y\n");

    p << aol;
    CHECK(p.toString() == without_delayed);

    p << L("");
    CHECK(p.toString() == with_delayed);
}

void test_and_or_list_with_rest(
        Printer::LineMode line_mode,
        and_or_list::synchronicity_type synchronicity,
        const xstring without_delayed,
        const xstring with_delayed) {
    and_or_list aol(pipeline_stub(), synchronicity);
    aol.rest().emplace_back(
            ConditionalPipeline::Condition::AND_THEN,
            ConditionalPipeline::PipelinePointer(
                    new Pipeline(pipeline_stub())));
    aol.rest().emplace_back(
            ConditionalPipeline::Condition::OR_ELSE,
            ConditionalPipeline::PipelinePointer(
                    new Pipeline(pipeline_stub())));

    Printer p(line_mode);
    p.indentLevel() = 1;
    p.delayedCharacters() << L("X");
    p.delayedLines() << L("Y\n");

    p << aol;
    CHECK(p.toString() == without_delayed);

    p << L("");
    CHECK(p.toString() == with_delayed);
}

TEST_CASE("And-or list constructor 1") {
    and_or_list aol(pipeline_stub());
    aol.rest().emplace_back(ConditionalPipeline::Condition::AND_THEN);
    aol.rest()[0].pipeline().commands().push_back(
            Pipeline::CommandPointer(new command_stub));
    aol.rest()[0].pipeline().commands().push_back(
            Pipeline::CommandPointer(new command_stub));

    CHECK(aol.synchronicity() == and_or_list::synchronicity_type::sequential);
    CHECK(aol.first().exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    CHECK(aol.first().commands().size() == 1);
    CHECK_NOTHROW((void)
            dynamic_cast<command_stub &>(*aol.first().commands()[0]));
    CHECK(aol.rest().size() == 1);

    const Pipeline &p1 = aol.rest()[0].pipeline();
    CHECK(p1.commands().size() == 2);
    CHECK_NOTHROW((void) dynamic_cast<command_stub &>(*p1.commands()[0]));
    CHECK_NOTHROW((void) dynamic_cast<command_stub &>(*p1.commands()[1]));
}

TEST_CASE("And-or list constructor 2") {
    and_or_list aol(
            Pipeline(Pipeline::ExitStatusType::NEGATED),
            and_or_list::synchronicity_type::asynchronous);

    CHECK(aol.first().commands().empty());
    CHECK(aol.first().exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    CHECK(aol.rest().empty());
    CHECK(aol.synchronicity() ==
            and_or_list::synchronicity_type::asynchronous);
}

TEST_CASE("And-or list print w/o rest") {
    test_and_or_list_without_rest(
            Printer::LineMode::SINGLE_LINE,
            and_or_list::synchronicity_type::sequential,
            L("X! command"),
            L("X! command; "));
    test_and_or_list_without_rest(
            Printer::LineMode::MULTI_LINE,
            and_or_list::synchronicity_type::sequential,
            L("X! command"),
            L("X! command; "));
    test_and_or_list_without_rest(
            Printer::LineMode::SINGLE_LINE,
            and_or_list::synchronicity_type::asynchronous,
            L("X! command&"),
            L("X! command& "));
    test_and_or_list_without_rest(
            Printer::LineMode::MULTI_LINE,
            and_or_list::synchronicity_type::asynchronous,
            L("X! command&"),
            L("X! command& "));
}

TEST_CASE("And-or list print w/ rest") {
    test_and_or_list_with_rest(
            Printer::LineMode::SINGLE_LINE,
            and_or_list::synchronicity_type::sequential,
            L("X! command && ! command || ! command"),
            L("X! command && ! command || ! command; "));
    test_and_or_list_with_rest(
            Printer::LineMode::MULTI_LINE,
            and_or_list::synchronicity_type::sequential,
            L("X! command &&\nY\n    ! command ||\n    ! command"),
            L("X! command &&\nY\n    ! command ||\n    ! command; "));
    test_and_or_list_with_rest(
            Printer::LineMode::SINGLE_LINE,
            and_or_list::synchronicity_type::asynchronous,
            L("X! command && ! command || ! command&"),
            L("X! command && ! command || ! command& "));
    test_and_or_list_with_rest(
            Printer::LineMode::MULTI_LINE,
            and_or_list::synchronicity_type::asynchronous,
            L("X! command &&\nY\n    ! command ||\n    ! command&"),
            L("X! command &&\nY\n    ! command ||\n    ! command& "));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
