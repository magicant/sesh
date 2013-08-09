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
#include <memory>
#include <stdexcept>
#include "language/Command.hh"
#include "language/ConditionalPipeline.hh"
#include "language/Pipeline.hh"
#include "language/Printer.hh"
#include "language/SourceLocationTestHelper.hh"

using sesh::language::Command;
using sesh::language::ConditionalPipeline;
using sesh::language::Pipeline;
using sesh::language::Printer;
using sesh::language::dummySourceLocation;

namespace {

class CommandStub : public Command {
private:
    std::wstring mString;
public:
    explicit CommandStub(const wchar_t *s) :
            Command(dummySourceLocation()), mString(s) { }
    void print(Printer &) const override;
};

void CommandStub::print(Printer &p) const {
    p << mString;
    p.delayedCharacters() << L' ';
}

void addCommand(Pipeline &p, const wchar_t *s) {
    p.commands().push_back(Pipeline::CommandPointer(new CommandStub(s)));
}

std::unique_ptr<Pipeline> newPipeline(const wchar_t *s) {
    std::unique_ptr<Pipeline> p(
            new Pipeline(Pipeline::ExitStatusType::NEGATED));
    addCommand(*p, s);
    return p;
}

} // namespace

TEST_CASE("Conditional pipeline constructors") {
    ConditionalPipeline cp1(ConditionalPipeline::Condition::AND_THEN);
    addCommand(cp1.pipeline(), L"pipeline 1");
    CHECK(cp1.condition() == ConditionalPipeline::Condition::AND_THEN);
    CHECK(cp1.pipeline().commands().size() == 1);

    ConditionalPipeline cp2(ConditionalPipeline::Condition::OR_ELSE);
    addCommand(cp2.pipeline(), L"pipeline 2");
    addCommand(cp2.pipeline(), L"pipeline 3");
    addCommand(cp2.pipeline(), L"pipeline 4");
    CHECK(cp2.condition() == ConditionalPipeline::Condition::OR_ELSE);
    CHECK(cp2.pipeline().commands().size() == 3);

    ConditionalPipeline cp3(
            ConditionalPipeline::Condition::AND_THEN,
            nullptr);
    addCommand(cp3.pipeline(), L"pipeline 5");
    CHECK(cp3.condition() == ConditionalPipeline::Condition::AND_THEN);
    CHECK(cp3.pipeline().commands().size() == 1);

    ConditionalPipeline cp4(
            ConditionalPipeline::Condition::OR_ELSE,
            nullptr);
    addCommand(cp4.pipeline(), L"pipeline 6");
    addCommand(cp4.pipeline(), L"pipeline 7");
    CHECK(cp4.condition() == ConditionalPipeline::Condition::OR_ELSE);
    CHECK(cp4.pipeline().commands().size() == 2);
}

TEST_CASE("Conditional pipeline print") {
    Printer ps(Printer::LineMode::SINGLE_LINE);
    Printer pm(Printer::LineMode::MULTI_LINE);

    ConditionalPipeline cp1(
            ConditionalPipeline::Condition::AND_THEN,
            newPipeline(L"pipeline 1"));

    ps.indentLevel() = 1;
    pm.indentLevel() = 1;
    ps << cp1;
    pm << cp1;
    CHECK(ps.toWstring() == L"&& ! pipeline 1");
    CHECK(pm.toWstring() == L"&&\n    ! pipeline 1");

    ConditionalPipeline cp2(
            ConditionalPipeline::Condition::OR_ELSE,
            newPipeline(L"pipeline 2"));

    ps.indentLevel() = 2;
    pm.indentLevel() = 2;
    ps << cp2;
    pm << cp2;
    CHECK(ps.toWstring() == L"&& ! pipeline 1 || ! pipeline 2");
    CHECK(pm.toWstring() == L"&&\n    ! pipeline 1 ||\n        ! pipeline 2");
}

/* vim: set et sw=4 sts=4 tw=79: */