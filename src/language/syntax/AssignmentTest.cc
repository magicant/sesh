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

#include <cstddef>
#include <stdexcept>
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::forEachLineMode;

struct Fixture {
    Assignment a;
    Fixture() : a() {
        a.variableName() = L("varName");
        a.value().addComponent(Word::ComponentPointer(
                new RawString(L("assigned"))));
        a.value().addComponent(Word::ComponentPointer(
                new RawString(L("Value"))));
    }
};

TEST_CASE("Assignment constructors") {
    Assignment a1;
    CHECK(a1.variableName() == L(""));
    CHECK(a1.value().components().size() == 0);

    xstring name(L("name"));
    Assignment a2(name, nullptr);
    CHECK(a2.variableName() == name);
    CHECK(a2.value().components().size() == 0);

    Assignment a3(xstring(name), nullptr);
    CHECK(a3.variableName() == name);
    CHECK(a3.value().components().size() == 0);

    Assignment a4(name, Assignment::WordPointer(new Word));
    CHECK(a4.variableName() == name);
    CHECK(a4.value().components().size() == 0);

    Assignment a5(xstring(name), Assignment::WordPointer(new Word));
    CHECK(a5.variableName() == name);
    CHECK(a5.value().components().size() == 0);
}

TEST_CASE_METHOD(Fixture, "Assignment data 1") {
    CHECK(a.variableName() == L("varName"));
}

TEST_CASE("Assignment data 2") {
    xstring name(L("name"));
    Assignment a1(name, Assignment::WordPointer(new Word));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L("value"))));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L(" "))));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L("string"))));
    CHECK(a1.variableName() == name);
    CHECK(a1.value().components().size() == 3);

    Assignment a2(xstring(name), Assignment::WordPointer(new Word));
    CHECK(a2.variableName() == name);
    CHECK(a2.value().components().size() == 0);
}

TEST_CASE_METHOD(Fixture, "Assignment print") {
    forEachLineMode([this](Printer &p) {
        p << a;
        CHECK(p.toString() == L("varName=assignedValue"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
