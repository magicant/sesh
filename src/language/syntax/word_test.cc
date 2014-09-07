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
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;
using sesh::language::syntax::word_component;

class non_constant : public word_component {
    bool append_constant_value(xstring &) const override { return false; }
    void print(printer &) const override { throw "unexpected print"; }
};

TEST_CASE("Word, constant value") {
    word w1, w2;
    REQUIRE(w1.maybe_constant_value().has_value());
    CHECK(w1.maybe_constant_value().value() == xstring());

    w1.add_component(word::component_pointer(new raw_string(L("ABC"))));
    REQUIRE(w1.maybe_constant_value().has_value());
    CHECK(w1.maybe_constant_value().value() == L("ABC"));

    w1.add_component(word::component_pointer(new raw_string(L("123"))));
    REQUIRE(w1.maybe_constant_value().has_value());
    CHECK(w1.maybe_constant_value().value() == L("ABC123"));

    w1.add_component(word::component_pointer(new non_constant));
    CHECK_FALSE(w1.maybe_constant_value().has_value());

    CHECK(w2.maybe_constant_value().has_value());
    w2.append(std::move(w1));
    CHECK_FALSE(w2.maybe_constant_value().has_value());
    REQUIRE(w1.maybe_constant_value().has_value());
    CHECK(w1.maybe_constant_value().value() == xstring());
}

TEST_CASE("Word, is raw string") {
    word w;
    CHECK(w.is_raw_string());

    w.add_component(word::component_pointer(new raw_string));
    CHECK(w.is_raw_string());

    w.add_component(word::component_pointer(new raw_string(L("Test"))));
    CHECK(w.is_raw_string());

    w.add_component(word::component_pointer(new non_constant));
    CHECK_FALSE(w.is_raw_string());
}

TEST_CASE("Word print") {
    for_each_line_mode([](printer &p) {
        word w;

        p << w;
        CHECK(p.to_string() == L(""));

        w.add_component(word::component_pointer(new raw_string(L("1"))));
        w.add_component(word::component_pointer(new raw_string(L("2"))));
        w.add_component(word::component_pointer(new raw_string(L("3"))));
        p << w;
        CHECK(p.to_string() == L("123"));

        p << L('X');
        CHECK(p.to_string() == L("123X")); // no delayed characters
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
