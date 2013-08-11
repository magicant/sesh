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

#include "common.hh"
#include <string>
#include "language/AndOrList.hh"
#include "language/Command.hh"
#include "language/ConditionalPipeline.hh"
#include "language/Pipeline.hh"
#include "language/Printer.hh"
#include "language/Sequence.hh"
#include "language/SourceLocationTestHelper.hh"

using sesh::language::AndOrList;
using sesh::language::Command;
using sesh::language::ConditionalPipeline;
using sesh::language::Pipeline;
using sesh::language::Printer;
using sesh::language::Sequence;
using sesh::language::dummySourceLocation;

namespace {

struct CommandStub : public Command {
    std::wstring s;
    CommandStub(std::wstring s) : Command(dummySourceLocation()), s(s) { }
    void print(Printer &p) const override {
        p << s;
        p.delayedCharacters() << L' ';
    }
};

struct PrintFixture {
    // C1 && C2; ! C3 | C4& C5
    Sequence s;
    PrintFixture() : s() {
        s.andOrLists().emplace_back(
                Sequence::AndOrListPointer(new AndOrList(Pipeline())));
        s.andOrLists()[0]->first().commands().emplace_back(
                Pipeline::CommandPointer(new CommandStub(L"C1")));
        s.andOrLists()[0]->rest().emplace_back(
                ConditionalPipeline::Condition::AND_THEN);
        s.andOrLists()[0]->rest()[0].pipeline().commands().emplace_back(
                Pipeline::CommandPointer(new CommandStub(L"C2")));

        s.andOrLists().emplace_back(
                Sequence::AndOrListPointer(new AndOrList(Pipeline())));
        s.andOrLists()[1]->synchronicity() =
                AndOrList::Synchronicity::ASYNCHRONOUS;
        s.andOrLists()[1]->first().exitStatusType() =
                Pipeline::ExitStatusType::NEGATED;
        s.andOrLists()[1]->first().commands().emplace_back(
                Pipeline::CommandPointer(new CommandStub(L"C3")));
        s.andOrLists()[1]->first().commands().emplace_back(
                Pipeline::CommandPointer(new CommandStub(L"C4")));

        s.andOrLists().emplace_back(
                Sequence::AndOrListPointer(new AndOrList(Pipeline())));
        s.andOrLists()[2]->first().commands().emplace_back(
                Pipeline::CommandPointer(new CommandStub(L"C5")));
    }
};

} // namespace

TEST_CASE_METHOD(PrintFixture, "Sequence print single-line") {
    Printer p(Printer::LineMode::SINGLE_LINE);
    p.delayedCharacters() << L'X';
    p.delayedLines() << L"Y\n";
    p.indentLevel() = 2;
    p << s;
    CHECK(p.toWstring() == L"XC1 && C2; ! C3 | C4& C5");
    p.commitDelayedCharacters();
    CHECK(p.toWstring() == L"XC1 && C2; ! C3 | C4& C5; ");
}

TEST_CASE_METHOD(PrintFixture, "Sequence print multi-line") {
    Printer p(Printer::LineMode::MULTI_LINE);
    p.delayedCharacters() << L'X';
    p.delayedLines() << L"Y\n";
    p.indentLevel() = 2;
    p << s;
    CHECK(p.toWstring() ==
            L"XC1 &&\nY\n        C2\n        ! C3 | C4&\n        C5");
    p.commitDelayedCharacters();
    CHECK(p.toWstring() ==
            L"XC1 &&\nY\n        C2\n        ! C3 | C4&\n        C5; ");
}

/* vim: set et sw=4 sts=4 tw=79: */