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
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::String;
using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::WordComponent;
using sesh::language::syntax::forEachLineMode;

class NonConstant : public WordComponent {
    bool appendConstantValue(String &) const override { return false; }
    void print(Printer &) const override { throw "unexpected print"; }
};

TEST_CASE("Word, constant value") {
    Word w1, w2;
    REQUIRE(w1.maybeConstantValue().hasValue());
    CHECK(w1.maybeConstantValue().value() == String());

    w1.addComponent(Word::ComponentPointer(new RawString(L("ABC"))));
    REQUIRE(w1.maybeConstantValue().hasValue());
    CHECK(w1.maybeConstantValue().value() == L("ABC"));

    w1.addComponent(Word::ComponentPointer(new RawString(L("123"))));
    REQUIRE(w1.maybeConstantValue().hasValue());
    CHECK(w1.maybeConstantValue().value() == L("ABC123"));

    w1.addComponent(Word::ComponentPointer(new NonConstant));
    CHECK_FALSE(w1.maybeConstantValue().hasValue());

    CHECK(w2.maybeConstantValue().hasValue());
    w2.append(std::move(w1));
    CHECK_FALSE(w2.maybeConstantValue().hasValue());
    REQUIRE(w1.maybeConstantValue().hasValue());
    CHECK(w1.maybeConstantValue().value() == String());
}

TEST_CASE("Word print") {
    forEachLineMode([](Printer &p) {
        Word w;

        p << w;
        CHECK(p.toString() == L(""));

        w.addComponent(Word::ComponentPointer(new RawString(L("1"))));
        w.addComponent(Word::ComponentPointer(new RawString(L("2"))));
        w.addComponent(Word::ComponentPointer(new RawString(L("3"))));
        p << w;
        CHECK(p.toString() == L("123"));

        p << L('X');
        CHECK(p.toString() == L("123X")); // no delayed characters
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
