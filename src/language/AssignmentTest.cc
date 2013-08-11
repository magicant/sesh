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
#include <stdexcept>
#include <string>
#include "language/Assignment.hh"
#include "language/Printer.hh"
#include "language/PrinterTestHelper.hh"
#include "language/RawString.hh"
#include "language/Word.hh"

using sesh::language::Assignment;
using sesh::language::Printer;
using sesh::language::RawString;
using sesh::language::Word;
using sesh::language::forEachLineMode;

namespace {

struct Fixture {
    Assignment a;
    Fixture() : a() {
        a.variableName() = L"varName";
        a.value().components().push_back(Word::ComponentPointer(
                new RawString(L"assigned")));
        a.value().components().push_back(Word::ComponentPointer(
                new RawString(L"Value")));
    }
};

} // namespace

TEST_CASE("Assignment constructors") {
    Assignment a1;
    CHECK(a1.variableName() == L"");
    CHECK(a1.value().components().size() == 0);

    std::wstring name(L"name");
    Assignment a2(name, nullptr);
    CHECK(a2.variableName() == name);
    CHECK(a2.value().components().size() == 0);

    Assignment a3(std::wstring(name), nullptr);
    CHECK(a3.variableName() == name);
    CHECK(a3.value().components().size() == 0);

    Assignment a4(name, Assignment::WordPointer(new Word));
    CHECK(a4.variableName() == name);
    CHECK(a4.value().components().size() == 0);

    Assignment a5((std::wstring(name)), Assignment::WordPointer(new Word));
    CHECK(a5.variableName() == name);
    CHECK(a5.value().components().size() == 0);
}

TEST_CASE_METHOD(Fixture, "Assignment data 1") {
    CHECK(a.variableName() == L"varName");
}

TEST_CASE("Assignment data 2") {
    std::wstring name(L"name");
    Assignment a1(name, Assignment::WordPointer(new Word));
    a1.value().components().push_back(
            Word::ComponentPointer(new RawString(L"value")));
    a1.value().components().push_back(
            Word::ComponentPointer(new RawString(L" ")));
    a1.value().components().push_back(
            Word::ComponentPointer(new RawString(L"string")));
    CHECK(a1.variableName() == name);
    CHECK(a1.value().components().size() == 3);

    Assignment a2(std::wstring(name), Assignment::WordPointer(new Word));
    CHECK(a2.variableName() == name);
    CHECK(a2.value().components().size() == 0);
}

TEST_CASE_METHOD(Fixture, "Assignment print") {
    forEachLineMode([this](Printer &p) {
        p << a;
        CHECK(p.toWstring() == L"varName=assignedValue");
    });
}

/* vim: set et sw=4 sts=4 tw=79: */
