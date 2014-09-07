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

#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"
#include "language/syntax/raw_string.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;
using sesh::language::syntax::raw_string;

TEST_CASE("Raw string constructors and value") {
    raw_string rs1;
    CHECK(rs1.value() == L(""));
    rs1.value() = L("test");
    CHECK(rs1.value() == L("test"));

    const raw_string rs2(rs1.value());
    CHECK(rs2.value() == L("test"));

    raw_string rs3(rs2.value());
    CHECK(rs3.value() == L("test"));
    rs3.value() += L(" string");
    CHECK(rs3.value() == L("test string"));
}

TEST_CASE("Raw string, append constant value") {
    xstring s;
    CHECK(raw_string(L("1")).append_constant_value(s));
    CHECK(s == L("1"));
    CHECK(raw_string(L("ABC")).append_constant_value(s));
    CHECK(s == L("1ABC"));
}

TEST_CASE("Raw string print") {
    for_each_line_mode([](printer &p) {
        p << raw_string(L("1")) << raw_string(L(""));
        p << raw_string(L("23"));
        CHECK(p.to_string() == L("123"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
