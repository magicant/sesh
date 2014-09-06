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
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;
using sesh::language::syntax::WordComponent;

class NonConstant : public WordComponent {
    bool appendConstantValue(xstring &) const override { return false; }
    void print(printer &) const override { throw "unexpected print"; }
};

TEST_CASE("Word, constant value") {
    Word w1, w2;
    REQUIRE(w1.maybeConstantValue().has_value());
    CHECK(w1.maybeConstantValue().value() == xstring());

    w1.addComponent(Word::ComponentPointer(new RawString(L("ABC"))));
    REQUIRE(w1.maybeConstantValue().has_value());
    CHECK(w1.maybeConstantValue().value() == L("ABC"));

    w1.addComponent(Word::ComponentPointer(new RawString(L("123"))));
    REQUIRE(w1.maybeConstantValue().has_value());
    CHECK(w1.maybeConstantValue().value() == L("ABC123"));

    w1.addComponent(Word::ComponentPointer(new NonConstant));
    CHECK_FALSE(w1.maybeConstantValue().has_value());

    CHECK(w2.maybeConstantValue().has_value());
    w2.append(std::move(w1));
    CHECK_FALSE(w2.maybeConstantValue().has_value());
    REQUIRE(w1.maybeConstantValue().has_value());
    CHECK(w1.maybeConstantValue().value() == xstring());
}

TEST_CASE("Word, is raw string") {
    Word w;
    CHECK(w.isRawString());

    w.addComponent(Word::ComponentPointer(new RawString));
    CHECK(w.isRawString());

    w.addComponent(Word::ComponentPointer(new RawString(L("Test"))));
    CHECK(w.isRawString());

    w.addComponent(Word::ComponentPointer(new NonConstant));
    CHECK_FALSE(w.isRawString());
}

TEST_CASE("Word print") {
    for_each_line_mode([](printer &p) {
        Word w;

        p << w;
        CHECK(p.to_string() == L(""));

        w.addComponent(Word::ComponentPointer(new RawString(L("1"))));
        w.addComponent(Word::ComponentPointer(new RawString(L("2"))));
        w.addComponent(Word::ComponentPointer(new RawString(L("3"))));
        p << w;
        CHECK(p.to_string() == L("123"));

        p << L('X');
        CHECK(p.to_string() == L("123X")); // no delayed characters
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
