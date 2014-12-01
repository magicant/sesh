/* Copyright (C) 2014 WATANABE Yuki
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

#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/printing/buffer.hh"
#include "language/printing/raw_string.hh"
#include "language/syntax/raw_string.hh"

namespace {

using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::raw_string;

TEST_CASE("Printing short raw string in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    xstring s = L("1");
    print(raw_string{s}, b);
    CHECK(b.to_string() == s);
}

TEST_CASE("Printing long raw string in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    xstring s = L("long_raw_string");
    print(raw_string{s}, b);
    CHECK(b.to_string() == s);
}

TEST_CASE("Printing long raw string in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    xstring s = L("long_raw_string");
    print(raw_string{s}, b);
    CHECK(b.to_string() == s);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
