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

#include <memory>
#include <utility>
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;
using sesh::language::syntax::word_component;

class non_constant : public word_component {
    bool append_constant_value(xstring &) const override { return false; }
};

TEST_CASE("Word, constant value") {
    word w1, w2;
    REQUIRE(w1.maybe_constant_value());
    CHECK(*w1.maybe_constant_value() == xstring());

    w1.add_component(word::component_pointer(new raw_string(L("ABC"))));
    REQUIRE(w1.maybe_constant_value());
    CHECK(*w1.maybe_constant_value() == L("ABC"));

    w1.add_component(word::component_pointer(new raw_string(L("123"))));
    REQUIRE(w1.maybe_constant_value());
    CHECK(*w1.maybe_constant_value() == L("ABC123"));

    w1.add_component(word::component_pointer(new non_constant));
    CHECK_FALSE(w1.maybe_constant_value());

    CHECK(w2.maybe_constant_value());
    w2.append(std::move(w1));
    CHECK_FALSE(w2.maybe_constant_value());
    REQUIRE(w1.maybe_constant_value());
    CHECK(*w1.maybe_constant_value() == xstring());
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

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
