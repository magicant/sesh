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

namespace {

using sesh::common::xstring;
using sesh::language::printing::buffer;

constexpr buffer::line_mode_type S = buffer::line_mode_type::single_line;
constexpr buffer::line_mode_type M = buffer::line_mode_type::multi_line;

TEST_CASE("Empty buffer yields empty string") {
    CHECK(buffer(S).to_string().empty());
    CHECK(buffer(M).to_string().empty());
}

void test_append_main(buffer::line_mode_type mode) {
    buffer b(mode);
    xstring s = L("ABC");
    b.append_main(s);
    CHECK(b.to_string() == s);
    b.append_main(s);
    CHECK(b.to_string() == s + s);
}

TEST_CASE("buffer::append_main in single line mode") {
    test_append_main(S);
}

TEST_CASE("buffer::append_main in multi-line mode") {
    test_append_main(M);
}

void test_append_main_with_delayed_characters(buffer::line_mode_type mode) {
    buffer b(mode);
    xstring d = L("delayed "), c = L("characters ");
    b.append_delayed_characters(d);
    b.append_main(c);
    CHECK(b.to_string() == d + c);
}

TEST_CASE("buffer::append_main with delayed characters in single line mode") {
    test_append_main_with_delayed_characters(S);
}

TEST_CASE("buffer::append_main with delayed characters in multi-line mode") {
    test_append_main_with_delayed_characters(M);
}

void test_append_main_clears_delayed_characters(buffer::line_mode_type mode) {
    buffer b(mode);
    xstring d = L("delayed "), c = L("characters ");
    b.append_delayed_characters(d);
    b.append_main(c);
    b.append_main(c);
    CHECK(b.to_string() == d + c + c);
}

TEST_CASE("buffer::append_main clears delayed characters in single line mode")
{
    test_append_main_clears_delayed_characters(S);
}

TEST_CASE("buffer::append_main clears delayed characters in multi-line mode") {
    test_append_main_clears_delayed_characters(M);
}

void test_clear_delayed_characters(buffer::line_mode_type mode) {
    buffer b(mode);
    xstring x = L("X"), y = L("Y");
    b.append_delayed_characters(x);
    b.clear_delayed_characters();
    b.append_main(y);
    CHECK(b.to_string() == y);
}

TEST_CASE("buffer::clear_delayed_characters in single line mode") {
    test_clear_delayed_characters(S);
}

TEST_CASE("buffer::clear_delayed_characters in multi-line mode") {
    test_clear_delayed_characters(M);
}

void test_commit_delayed_characters(buffer:: line_mode_type mode) {
    buffer b(mode);
    xstring s = L("ABC");
    b.append_delayed_characters(s);
    b.commit_delayed_characters();
    CHECK(b.to_string() == s);
}

TEST_CASE("buffer::commit_delayed_characters in single line mode") {
    test_commit_delayed_characters(S);
}

TEST_CASE("buffer::commit_delayed_characters in multi-line mode") {
    test_commit_delayed_characters(M);
}

TEST_CASE("buffer::break_line retains main buffer in single line mode") {
    buffer b(S);
    xstring m = L("main");
    b.append_main(m);
    b.break_line();
    CHECK(b.to_string() == m);
}

TEST_CASE(
        "buffer::break_line retains delayed characters in single line mode") {
    buffer b(S);
    xstring s = L("delayed");
    b.append_delayed_characters(s);
    b.break_line();
    CHECK(b.to_string().empty());
    b.commit_delayed_characters();
    CHECK(b.to_string() == s);
}

TEST_CASE("buffer::break_line ignores delayed lines in single line mode") {
    buffer b(S);
    xstring m = L("main");
    b.append_main(m);
    b.append_delayed_lines(L("lines"));
    b.break_line();
    b.append_main(m);
    CHECK(b.to_string() == m + m);
}

TEST_CASE("buffer::break_line inserts newline in multi-line mode") {
    buffer b(M);
    xstring pre = L("A"), post = L("B");
    b.append_main(pre);
    b.break_line();
    b.append_main(post);
    CHECK(b.to_string() == L("A\nB"));
}

TEST_CASE("buffer::break_line resets delayed characters in multi-line mode") {
    buffer b(M);
    xstring s = L("test");
    b.append_delayed_characters(L("ABC"));
    b.break_line();
    b.append_main(s);
    CHECK(b.to_string() == L("\n") + s);
}

TEST_CASE("buffer::break_line inserts delayed lines in multi-line mode") {
    buffer b(M);
    xstring d = L("delayed "), l = L("line");
    b.append_delayed_lines(d);
    b.append_delayed_lines(l);
    b.break_line();
    CHECK(b.to_string() == L("\n") + d + l);
}

TEST_CASE("buffer::break_line resets delayed lines in multi-line mode") {
    buffer b(M);
    xstring l = L("line");
    b.append_delayed_lines(l);
    b.break_line();
    b.break_line();
    CHECK(b.to_string() == L("\n") + l + L("\n"));
}

TEST_CASE("buffer::indent retains main buffer in single line mode") {
    buffer b(S);
    xstring pre = L("A"), post = L("B");
    b.append_main(pre);
    b.indent();
    b.append_main(post);
    CHECK(b.to_string() == pre + post);
}

TEST_CASE("buffer::indent retains delayed characters in single line mode") {
    buffer b(S);
    xstring d = L("D"), m = L("M");
    b.append_delayed_characters(d);
    b.indent();
    b.append_main(m);
    CHECK(b.to_string() == d + m);
}

TEST_CASE("buffer::indent inserts spaces in multi-line mode") {
    buffer b(M);
    xstring pre = L("A"), post = L("B");
    b.append_main(pre);
    b.indent_level() = 3;
    b.indent();
    CHECK(b.to_string() == pre);
    b.append_main(post);
    CHECK(b.to_string() == pre + xstring(12, L(' ')) + post);
}

TEST_CASE("buffer::indent retains main buffer in multi-line mode") {
    buffer b(M);
    xstring pre = L("A");
    b.append_main(pre);
    b.indent_level() = 3;
    b.indent();
    CHECK(b.to_string() == pre);
}

TEST_CASE("buffer::indent retains delayed lines in multi-line mode") {
    buffer b(M);
    xstring l = L("line");
    b.append_delayed_lines(l);
    b.indent_level() = 1;
    b.indent();
    b.break_line();
    CHECK(b.to_string() == L("\n") + l);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
