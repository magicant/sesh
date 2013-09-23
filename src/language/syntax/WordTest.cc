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

#include <memory>
#include "common/Char.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

namespace {

using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::forEachLineMode;

TEST_CASE("Word print") {
    forEachLineMode([](Printer &p) {
        Word w;

        p << w;
        CHECK(p.toString() == L(""));

        w.components().emplace_back(new RawString(L("1")));
        w.components().emplace_back(new RawString(L("2")));
        w.components().emplace_back(new RawString(L("3")));
        p << w;
        CHECK(p.toString() == L("123"));

        p << L('X');
        CHECK(p.toString() == L("123X")); // no delayed characters
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
