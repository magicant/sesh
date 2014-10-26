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
#include "language/printing/sequence.hh"
#include "language/syntax/and_or_list_test_helper.hh"
#include "language/syntax/sequence.hh"
#include "language/syntax/sequence_test_helper.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::make_and_or_list_stub;
using sesh::language::syntax::make_sequence_stub;
using sesh::language::syntax::sequence;
using sesh::language::printing::buffer;

const xstring s1 = L("first"), s2 = L("second"), s3 = L("third");

TEST_CASE("Printing single list sequence in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    sequence s = make_sequence_stub(s1);
    print(s, b);
    CHECK(b.to_string() == s1);
}

TEST_CASE("Printing double list sequence in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    sequence s = make_sequence_stub(s1);
    s.and_or_lists.push_back(make_and_or_list_stub(s2));
    print(s, b);
    CHECK(b.to_string() == s1 + L("; ") + s2);
}

TEST_CASE("Printing single list sequence in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    sequence s = make_sequence_stub(s1);
    print(s, b);
    CHECK(b.to_string() == s1);
}

TEST_CASE("And-or lists are newline-separated in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    sequence s = make_sequence_stub(s1);
    s.and_or_lists.push_back(make_and_or_list_stub(s2));
    s.and_or_lists.push_back(make_and_or_list_stub(s3));
    print(s, b);
    CHECK(b.to_string() == s1 + L("\n") + s2 + L("\n") + s3);
}

TEST_CASE("Indentation in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    sequence s = make_sequence_stub(s1);
    s.and_or_lists.push_back(make_and_or_list_stub(s2));
    b.indent_level() = 3;
    print(s, b);
    CHECK(b.to_string() == s1 + L("\n") + xstring(3 * 4, L(' ')) + s2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
