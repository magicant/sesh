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
#include "language/parsing/comment.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/parser_test_helper.hh"

namespace {

using sesh::common::xstring;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_no_reports;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::skip_comment;

TEST_CASE("skip_comment fails at end of input") {
    check_parser_failure(skip_comment, {});
}

TEST_CASE("skip_comment fails with alphabet") {
    check_parser_failure(skip_comment, L("a"));
}

TEST_CASE("Failed skip_comment reports nothing") {
    check_parser_no_reports(skip_comment, {});
}

TEST_CASE("skip_comment succeeds with # followed by end of input") {
    check_parser_success_result(
            skip_comment, L("#"), [](const xstring &s) { CHECK(s.empty()); });
}

TEST_CASE("skip_comment returns comment body (w/o newline)") {
    check_parser_success_result(
            skip_comment,
            L("# hello world"),
            [](const xstring &s) { CHECK(s == L(" hello world")); });
}

TEST_CASE("skip_comment consumes comment") {
    check_parser_success_rest(skip_comment, L("#hello"));
}

TEST_CASE("skip_comment stops at newline (result)") {
    check_parser_success_result(
            skip_comment,
            L("#hello\n"),
            [](const xstring &s) { CHECK(s == L("hello")); });
}

TEST_CASE("skip_comment stops at newline (rest)") {
    check_parser_success_rest(skip_comment, L("#hello"), L("\n"));
}

TEST_CASE("Successful skip_comment reports nothing") {
    check_parser_no_reports(skip_comment, L("#hello\n"));
}

TEST_CASE("skip_comment is context-free") {
    check_parser_success_context_free(skip_comment, L("#hello\n"));
}

TEST_CASE("Comment body cannot include line continuation") {
    check_parser_success_result(
            skip_comment,
            L("#\\\n"),
            [](const xstring &s) { CHECK(s == L("\\")); });
    // TODO require warning in this case?
}

TEST_CASE("skip_comment doesn't support preceding line continuation") {
    check_parser_failure(skip_comment, L("\\\n#"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
