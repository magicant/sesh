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
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Printer;
using sesh::language::syntax::forEachLineMode;

class CommandStub : public command {
private:
    xstring mString;
public:
    explicit CommandStub(const xchar *s) : command(), mString(s) { }
    void print(Printer &p) const override {
        p << mString;
        p.delayedCharacters() << L(' ');
    }
};

void addCommand(Pipeline &p, const xchar *s) {
    p.commands().push_back(Pipeline::CommandPointer(new CommandStub(s)));
}

void checkForEachLineMode(const Pipeline &pl, const xstring &expected) {
    forEachLineMode([&pl, &expected](Printer &p) {
        p << pl;
        CHECK(p.toString() == expected);
    });
}

TEST_CASE("Pipeline print") {
    Pipeline pl;

    addCommand(pl, L("Command 1"));

    checkForEachLineMode(pl, L("Command 1"));

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    pl.exitStatusType() = Pipeline::ExitStatusType::NEGATED;

    checkForEachLineMode(pl, L("! Command 1"));

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    addCommand(pl, L("Command 2"));

    checkForEachLineMode(pl, L("! Command 1 | Command 2"));

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::NEGATED);
    pl.exitStatusType() = Pipeline::ExitStatusType::STRAIGHT;

    checkForEachLineMode(pl, L("Command 1 | Command 2"));

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    addCommand(pl, L("The last command"));

    checkForEachLineMode(pl, L("Command 1 | Command 2 | The last command"));

    CHECK(pl.exitStatusType() == Pipeline::ExitStatusType::STRAIGHT);
    pl.exitStatusType() = Pipeline::ExitStatusType::NEGATED;

    checkForEachLineMode(pl, L("! Command 1 | Command 2 | The last command"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
