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
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/conditional_pipeline_test_helper.hh"
#include "language/printing/buffer.hh"
#include "language/printing/conditional_pipeline.hh"

namespace {

using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::make_conditional_pipeline_stub;

void check_prefix(
        buffer::line_mode_type l,
        conditional_pipeline::condition_type c,
        const xstring &expected_prefix,
        const xstring &command_string) {
    buffer b(l);
    conditional_pipeline p = make_conditional_pipeline_stub(command_string);
    p.condition = c;
    print(p, b);
    CHECK(b.to_string() == expected_prefix + command_string);
}

constexpr auto S = buffer::line_mode_type::single_line;
constexpr auto M = buffer::line_mode_type::multi_line;
constexpr auto A = conditional_pipeline::condition_type::and_then;
constexpr auto O = conditional_pipeline::condition_type::or_else;

TEST_CASE("And-then pipeline is preceded by && in single line mode") {
    check_prefix(S, A, L("&& "), L("test"));
}

TEST_CASE("Or-else pipeline is preceded by || in single line mode") {
    check_prefix(S, O, L("|| "), L("test"));
}

TEST_CASE("And-then pipeline is preceded by && in multi-line mode") {
    check_prefix(M, A, L("&&\n"), L("command"));
}

TEST_CASE("Or-else pipeline is preceded by || in multi-line mode") {
    check_prefix(M, O, L("||\n"), L("command"));
}

TEST_CASE("&& is followed by delayed lines in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    const xstring command = L("command");
    const xstring line = L("Delayed line\n");
    conditional_pipeline p = make_conditional_pipeline_stub(command);
    b.append_delayed_lines(line);
    print(p, b);
    CHECK(b.to_string() == L("&&\n") + line + command);
}

TEST_CASE("Pipeline is indented in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    const xstring command = L("command");
    conditional_pipeline p = make_conditional_pipeline_stub(command);
    b.indent_level() = 2;
    print(p, b);
    CHECK(b.to_string() == L("&&\n") + xstring(2 * 4, L(' ')) + command);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
