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
#include "language/syntax/assignment.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::assignment;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::forEachLineMode;

struct fixture {
    assignment a;
    fixture() : a() {
        a.variable_name() = L("varName");
        a.value().addComponent(Word::ComponentPointer(
                new RawString(L("assigned"))));
        a.value().addComponent(Word::ComponentPointer(
                new RawString(L("Value"))));
    }
};

TEST_CASE("Assignment constructors") {
    assignment a1;
    CHECK(a1.variable_name() == L(""));
    CHECK(a1.value().components().size() == 0);

    xstring name(L("name"));
    assignment a2(name, nullptr);
    CHECK(a2.variable_name() == name);
    CHECK(a2.value().components().size() == 0);

    assignment a3(xstring(name), nullptr);
    CHECK(a3.variable_name() == name);
    CHECK(a3.value().components().size() == 0);

    assignment a4(name, assignment::word_pointer(new Word));
    CHECK(a4.variable_name() == name);
    CHECK(a4.value().components().size() == 0);

    assignment a5(xstring(name), assignment::word_pointer(new Word));
    CHECK(a5.variable_name() == name);
    CHECK(a5.value().components().size() == 0);
}

TEST_CASE_METHOD(fixture, "Assignment data 1") {
    CHECK(a.variable_name() == L("varName"));
}

TEST_CASE("Assignment data 2") {
    xstring name(L("name"));
    assignment a1(name, assignment::word_pointer(new Word));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L("value"))));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L(" "))));
    a1.value().addComponent(
            Word::ComponentPointer(new RawString(L("string"))));
    CHECK(a1.variable_name() == name);
    CHECK(a1.value().components().size() == 3);

    assignment a2(xstring(name), assignment::word_pointer(new Word));
    CHECK(a2.variable_name() == name);
    CHECK(a2.value().components().size() == 0);
}

TEST_CASE_METHOD(fixture, "Assignment print") {
    forEachLineMode([this](Printer &p) {
        p << a;
        CHECK(p.toString() == L("varName=assignedValue"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
