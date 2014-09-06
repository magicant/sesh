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

#include <utility>
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/assignment.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::assignment;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;
using sesh::language::syntax::forEachLineMode;

assignment::word_pointer newWord(xstring s) {
    assignment::word_pointer w(new Word);
    w->addComponent(Word::ComponentPointer(new RawString(s)));
    return w;
}

SimpleCommand::AssignmentPointer newAssignment(xstring name, xstring value) {
    return SimpleCommand::AssignmentPointer(
            new assignment(name, newWord(value)));
}

void addAssignment(SimpleCommand &sc, xstring name, xstring value) {
    sc.assignments().push_back(newAssignment(name, value));
}

void addWord(SimpleCommand &sc, xstring s) {
    sc.words().push_back(newWord(s));
}

TEST_CASE("Empty simple command print") {
    forEachLineMode([](Printer &p) {
        p << SimpleCommand();
        CHECK(p.toString() == L(""));
    });
}

TEST_CASE("Simple command print") {
    SimpleCommand sc;

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
