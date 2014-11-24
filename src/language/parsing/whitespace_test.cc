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
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/whitespace.hh"

namespace {

using sesh::language::parsing::skip_whitespaces;

TEST_CASE("skip_whitespaces succeeds with empty input") {
    check_parser_success_rest(skip_whitespaces, {});
}

TEST_CASE("skip_whitespaces skips spaces and tabs") {
    check_parser_success_rest(skip_whitespaces, L(" \t \t"));
}

TEST_CASE("skip_whitespaces skips line continuations") {
    check_parser_success_rest(skip_whitespaces, L("\\\n \\\n"));
}

TEST_CASE("skip_whitespaces skips comment") {
    check_parser_success_rest(skip_whitespaces, L("#\\"), L("\n"));
}

TEST_CASE("skip_whitespaces doesn't skip non-whitespaces") {
    check_parser_success_rest(skip_whitespaces, L(" \t"), L("a"));
}

TEST_CASE("skip_whitespaces: blanks, line continuations, comment") {
    check_parser_success_rest(skip_whitespaces, L("\\\n\t \\\n#\\"), L("\n"));
}

TEST_CASE("skip_whitespaces is context-free") {
    check_parser_success_context_free(skip_whitespaces, L("\\\n\t \\\n#\\\n"));
}

TEST_CASE("skip_whitespaces doesn't read more than newline") {
    check_parser_no_excess(skip_whitespaces, L("\\\n\t \\\n#\\\n"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
