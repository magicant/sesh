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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common.hh"
#include <memory>
#include "language/Printer.hh"
#include "language/PrinterTestHelper.hh"
#include "language/RawString.hh"
#include "language/Word.hh"

using sesh::language::Printer;
using sesh::language::RawString;
using sesh::language::Word;
using sesh::language::forEachLineMode;

TEST_CASE("Word print") {
    forEachLineMode([](Printer &p) {
        Word w;

        p << w;
        CHECK(p.toWstring() == L"");

        w.components().push_back(Word::ComponentPointer(new RawString(L"1")));
        w.components().push_back(Word::ComponentPointer(new RawString(L"2")));
        w.components().push_back(Word::ComponentPointer(new RawString(L"3")));
        p << w;
        CHECK(p.toWstring() == L"123");

        p << L'X';
        CHECK(p.toWstring() == L"123X"); // no delayed characters
    });
}

/* vim: set et sw=4 sts=4 tw=79: */