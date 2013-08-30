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
#include <cstddef>
#include <memory>
#include <string>
#include "language/syntax/Command.hh"
#include "language/syntax/Printer.hh"
#include "parser/SourceLocation.hh"

using sesh::language::syntax::Command;
using sesh::language::syntax::Printer;
using sesh::parser::SourceLocation;

namespace {

class CommandStub : public Command {
public:
    using Command::Command;
    void print(Printer &) const override;
};

void CommandStub::print(Printer &) const {
    CHECK(false);
}

void testSourceLocation(
        const wchar_t *name,
        std::size_t line,
        std::size_t column) {
    SourceLocation sl(std::make_shared<std::wstring>(name), line, column);
    CommandStub dl(sl);
    CHECK(dl.sourceLocation().name() == sl.name());
    CHECK(dl.sourceLocation().line() == sl.line());
    CHECK(dl.sourceLocation().column() == sl.column());

    dl = CommandStub(SourceLocation(sl));
    CHECK(dl.sourceLocation().name() == sl.name());
    CHECK(dl.sourceLocation().line() == sl.line());
    CHECK(dl.sourceLocation().column() == sl.column());
}

} // namespace

TEST_CASE("Command source location") {
    testSourceLocation(L"", 0, 0);
    testSourceLocation(L"foo", 1, 1);
    testSourceLocation(L"foobar", 1234, 9876);
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
