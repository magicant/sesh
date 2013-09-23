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

#include "common/Char.hh"
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"
#include "language/syntax/RawString.hh"

namespace {

using sesh::language::syntax::Printer;
using sesh::language::syntax::RawString;
using sesh::language::syntax::forEachLineMode;

TEST_CASE("Raw string constructors and value") {
    RawString rs1;
    CHECK(rs1.value() == L(""));
    rs1.value() = L("test");
    CHECK(rs1.value() == L("test"));

    const RawString rs2(rs1.value());
    CHECK(rs2.value() == L("test"));

    RawString rs3(rs2.value());
    CHECK(rs3.value() == L("test"));
    rs3.value() += L(" string");
    CHECK(rs3.value() == L("test string"));
}

TEST_CASE("Raw string print") {
    forEachLineMode([](Printer &p) {
        p << RawString(L("1")) << RawString(L(""));
        p << RawString(L("23"));
        CHECK(p.toString() == L("123"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
