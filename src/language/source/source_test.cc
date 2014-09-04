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

#include <memory>
#include <stdexcept>
#include "common/xchar.hh"
#include "language/source/source.hh"
#include "language/source/source_test_helper.hh"

namespace {

using sesh::language::source::source;
using sesh::language::source::source_stub;

TEST_CASE("Source construction") {
    source::source_pointer s;

    CHECK_NOTHROW(s.reset(new source_stub(nullptr, 0, 0, L(""))));
    CHECK_NOTHROW(s.reset(new source_stub(std::move(s), 0, 0, L("test"))));
    CHECK_NOTHROW(s.reset(new source_stub(std::move(s), 0, 4, L(""))));
}

TEST_CASE("Source assignment") {
    source::source_pointer s(new source_stub(nullptr, 0, 0, L("")));
    source_stub ss(std::move(s), 0, 0, L(""));
    CHECK_NOTHROW(ss = source_stub(nullptr, 0, 0, L("")));
}

TEST_CASE("Source construction exception") {
    source::source_pointer s;

    CHECK_THROWS_AS(source_stub(nullptr, 1, 0, L("test")), std::out_of_range);
    CHECK_THROWS_AS(source_stub(nullptr, 0, 1, L("test")), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("test")));
    CHECK_THROWS_AS(source_stub(std::move(s), 3, 2, L("")), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("test")));
    CHECK_THROWS_AS(source_stub(std::move(s), 0, 5, L("")), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("test")));
    CHECK_THROWS_AS(source_stub(std::move(s), 5, 0, L("")), std::out_of_range);
}

TEST_CASE("Source length") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("")));
    CHECK(s->length() == 0);

    s.reset(new source_stub(nullptr, 0, 0, L("test")));
    CHECK(s->length() == 4);

    s.reset(new source_stub(std::move(s), 1, 3, L("test")));
    CHECK(s->length() == 6);

    s.reset(new source_stub(std::move(s), 0, 6, L("")));
    CHECK(s->length() == 0);
}

TEST_CASE("Source at and operator[]") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("")));
    INFO("source=''");
    check_source_string(*s, L(""));

    s.reset(new source_stub(std::move(s), 0, 0, L("x")));
    INFO("source='x'");
    check_source_string(*s, L("x"));

    s.reset(new source_stub(std::move(s), 0, 0, L("t")));
    INFO("source='tx'");
    check_source_string(*s, L("tx"));

    s.reset(new source_stub(std::move(s), 2, 2, L("t")));
    INFO("source='txt'");
    check_source_string(*s, L("txt"));

    s.reset(new source_stub(std::move(s), 1, 2, L("es")));
    INFO("source='test'");
    check_source_string(*s, L("test"));

    s.reset(new source_stub(std::move(s), 1, 3, L("")));
    INFO("source='tt'");
    check_source_string(*s, L("tt"));
}

TEST_CASE("Source line begin") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("abc\nxyz\n")));
    INFO("source='abc\\nxyz\\n'");
    check_source_line_begin(*s, {4, 8});

    s.reset(new source_stub(std::move(s), 3, 4, L("d\n\nw")));
    INFO("source='abcd\\n\\nwxyz\\n'");
    check_source_line_begin(*s, {5, 6, 11});

    s.reset(new source_stub(nullptr, 0, 0, L("\n\n")));
    INFO("source='\\n\\n'");
    check_source_line_begin(*s, {1, 2});

    s.reset(new source_stub(std::move(s), 1, 1, L("\n\n")));
    INFO("source='\\n\\n\\n\\n'");
    check_source_line_begin(*s, {1, 2, 3, 4});

    s.reset(new source_stub(std::move(s), 1, 3, L("")));
    INFO("source='\\n\\n'");
    check_source_line_begin(*s, {1, 2});

    s.reset(new source_stub(nullptr, 0, 0, L("abc\nxyz")));
    s.reset(new source_stub(std::move(s), 2, 5, L("p")));
    INFO("source='abpyz'");
    check_source_line_begin(*s, {});
}

TEST_CASE("Source line end") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("abc\nxyz\n")));
    INFO("source='abc\\nxyz\\n'");
    check_source_line_end(*s, {4, 8});

    // L("abcd\n\nwxyz\n")
    s.reset(new source_stub(std::move(s), 3, 4, L("d\n\nw")));
    INFO("source='abcd\\n\\nwxyz\\n'");
    check_source_line_end(*s, {5, 6, 11});

    s.reset(new source_stub(nullptr, 0, 0, L("\n\n")));
    INFO("source='\\n\\n'");
    check_source_line_end(*s, {1, 2});

    s.reset(new source_stub(std::move(s), 1, 1, L("\n\n")));
    INFO("source='\\n\\n\\n\\n'");
    check_source_line_end(*s, {1, 2, 3, 4});

    s.reset(new source_stub(std::move(s), 1, 3, L("")));
    INFO("source='\\n\\n'");
    check_source_line_end(*s, {1, 2});

    s.reset(new source_stub(nullptr, 0, 0, L("abc\nxyz")));
    s.reset(new source_stub(std::move(s), 2, 5, L("p")));
    INFO("source='abpyz'");
    check_source_line_end(*s, {5});

    s.reset(new source_stub(std::move(s), 5, 5, L("\n")));
    INFO("source='abpyz\\n'");
    check_source_line_end(*s, {6});
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
