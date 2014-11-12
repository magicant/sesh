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
#include "common/either.hh"
#include "common/xchar.hh"
#include "language/parsing/eof.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/source/fragment.hh"
#include "language/source/stream.hh"

namespace {

using sesh::common::trial;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_no_reports;
using sesh::language::parsing::check_parser_success_context_free;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::context;
using sesh::language::parsing::eof;
using sesh::language::parsing::parse_eof;
using sesh::language::parsing::result;
using sesh::language::parsing::state;
using sesh::language::source::empty_stream;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::source::stream_of;

TEST_CASE("EOF parser succeeds at EOF") {
    check_parser_success_result(parse_eof, {}, [](eof) { });
}

TEST_CASE("EOF parser reports nothing on success") {
    check_parser_no_reports(parse_eof, {});
}

TEST_CASE("EOF parser is context-free") {
    check_parser_success_context_free(parse_eof, {});
}

TEST_CASE("EOF parser fails with non-empty input") {
    check_parser_failure(parse_eof, L("X"));
}

TEST_CASE("EOF parser peeks only one char on failure") {
    check_parser_no_excess(parse_eof, L("X"));
}

TEST_CASE("EOF parser reports nothing on failure") {
    check_parser_no_reports(parse_eof, L("X"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
