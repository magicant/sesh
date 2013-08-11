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
#include <utility>
#include "language/Assignment.hh"
#include "language/Printer.hh"
#include "language/PrinterTestHelper.hh"
#include "language/RawString.hh"
#include "language/SimpleCommand.hh"
#include "language/SourceLocationTestHelper.hh"
#include "language/Word.hh"
#include "language/WordComponent.hh"

using sesh::language::Assignment;
using sesh::language::Printer;
using sesh::language::RawString;
using sesh::language::SimpleCommand;
using sesh::language::Word;
using sesh::language::dummySourceLocation;
using sesh::language::forEachLineMode;

namespace {

Assignment::WordPointer newWord(std::wstring s) {
    Assignment::WordPointer w(new Word);
    w->components().push_back(Word::ComponentPointer(new RawString(s)));
    return std::move(w);
}

SimpleCommand::AssignmentPointer newAssignment(
        std::wstring name, std::wstring value) {
    return SimpleCommand::AssignmentPointer(
            new Assignment(name, newWord(value)));
}

void addAssignment(SimpleCommand &sc, std::wstring name, std::wstring value) {
    sc.assignments().push_back(newAssignment(name, value));
}

void addWord(SimpleCommand &sc, std::wstring s) {
    sc.words().push_back(newWord(s));
}

} // namespace

TEST_CASE("Empty simple command print") {
    forEachLineMode([](Printer &p) {
        p << SimpleCommand(dummySourceLocation());
        CHECK(p.toWstring() == L"");
    });
}

TEST_CASE("Simple command print") {
    SimpleCommand sc(dummySourceLocation());

    addAssignment(sc, L"foo", L"Foo.value");
    addWord(sc, L"Hello");
    addAssignment(sc, L"bar", L"Bar-value");
    addWord(sc, L"world");

    forEachLineMode([&sc](Printer &p) {
        p << sc;
        CHECK(p.toWstring() == L"foo=Foo.value bar=Bar-value Hello world");
    });
}

/* vim: set et sw=4 sts=4 tw=79: */
