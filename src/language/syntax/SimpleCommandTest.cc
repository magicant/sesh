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

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/SourceLocationTestHelper.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::String;
using sesh::language::source::dummySourceLocation;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;
using sesh::language::syntax::forEachLineMode;

Assignment::WordPointer newWord(String s) {
    Assignment::WordPointer w(new Word);
    w->components().push_back(Word::ComponentPointer(new RawString(s)));
    return std::move(w);
}

SimpleCommand::AssignmentPointer newAssignment(String name, String value) {
    return SimpleCommand::AssignmentPointer(
            new Assignment(name, newWord(value)));
}

void addAssignment(SimpleCommand &sc, String name, String value) {
    sc.assignments().push_back(newAssignment(name, value));
}

void addWord(SimpleCommand &sc, String s) {
    sc.words().push_back(newWord(s));
}

TEST_CASE("Empty simple command print") {
    forEachLineMode([](Printer &p) {
        p << SimpleCommand(dummySourceLocation());
        CHECK(p.toString() == L(""));
    });
}

TEST_CASE("Simple command print") {
    SimpleCommand sc(dummySourceLocation());

    addAssignment(sc, L("foo"), L("Foo.value"));
    addWord(sc, L("Hello"));
    addAssignment(sc, L("bar"), L("Bar-value"));
    addWord(sc, L("world"));

    forEachLineMode([&sc](Printer &p) {
        p << sc;
        CHECK(p.toString() == L("foo=Foo.value bar=Bar-value Hello world"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
