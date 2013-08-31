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
#include <string>
#include "language/source/SourceLocationTestHelper.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"

using sesh::language::source::dummySourceLocation;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;
using sesh::language::syntax::forEachLineMode;

namespace {

class CommandStub : public Command {
private:
    std::wstring mString;
public:
    explicit CommandStub(const wchar_t *s) :
            Command(dummySourceLocation()), mString(s) { }
    void print(Printer &p) const override {
        p << mString;
        p.delayedCharacters() << L' ';
    }
};

void addCommand(Pipeline &p, const wchar_t *s) {
    p.commands().push_back(Pipeline::CommandPointer(new CommandStub(s)));
}

void checkForEachLineMode(const Pipeline &pl, const std::wstring &expected) {
    forEachLineMode([&pl, &expected](Printer &p) {
        p << pl;
        CHECK(p.toWstring() == expected);
    });
}

} // namespace

TEST_CASE("Pipeline print") {
    Pipeline pl;

    addCommand(pl, L"Command 1");

    checkForEachLineMode(pl, L"Command 1");

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    pl.exitStatusType() = Pipeline::ExitStatusType::NEGATED;

    checkForEachLineMode(pl, L"! Command 1");

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    addCommand(pl, L"Command 2");

    checkForEachLineMode(pl, L"! Command 1 | Command 2");

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    pl.exitStatusType() = Pipeline::ExitStatusType::STRAIGHT;

    checkForEachLineMode(pl, L"Command 1 | Command 2");

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    addCommand(pl, L"The last command");

    checkForEachLineMode(pl, L"Command 1 | Command 2 | The last command");

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    pl.exitStatusType() = Pipeline::ExitStatusType::NEGATED;

    checkForEachLineMode(pl, L"! Command 1 | Command 2 | The last command");
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
