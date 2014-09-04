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

#include <utility>
#include "common/xchar.hh"
#include "language/source/line_appended_source.hh"
#include "language/source/location_test_helper.hh"
#include "language/source/source_test_helper.hh"

namespace {

using sesh::language::source::check_source_line_end;
using sesh::language::source::dummyLineLocation;
using sesh::language::source::line_appended_source;
using sesh::language::source::line_location;
using sesh::language::source::source;
using sesh::language::source::source_stub;

using pointer = sesh::language::source::line_appended_source::source_pointer;
using string = sesh::language::source::line_appended_source::string_type;

line_appended_source *create(
        pointer &&original, string &&line, line_location &&location) {
    return new line_appended_source(line_appended_source::create(
                std::move(original), std::move(line), std::move(location)));
}

TEST_CASE("Line-appended source construction no throw") {
    source::source_pointer s;

    CHECK_NOTHROW(s.reset(create(
            nullptr, L(""), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("\n"), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("test\n"), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("more"), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("line"), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("last\n"), dummyLineLocation())));

    CHECK_NOTHROW(s.reset(create(
            nullptr, L("txt\n"), dummyLineLocation())));
    CHECK_NOTHROW(s.reset(new source_stub(std::move(s), 1, 2, L("es"))));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("next\n"), dummyLineLocation())));
}

TEST_CASE("Line-appended source construction throw") {
    source::source_pointer s;

    CHECK_THROWS_AS(
            s.reset(create(nullptr, L("t\next"), dummyLineLocation())),
            std::invalid_argument);

    CHECK_THROWS_AS(
            s.reset(create(nullptr, L("\n\n"), dummyLineLocation())),
            std::invalid_argument);
}

TEST_CASE("Line-appended source assignment") {
    line_appended_source las = line_appended_source::create(
            nullptr, L(""), dummyLineLocation());
    las = line_appended_source::create(
            nullptr, L(""), dummyLineLocation());
}

TEST_CASE("Line-appended source value") {
    source::source_pointer s;

    s.reset(create(nullptr, L(""), dummyLineLocation()));
    INFO("source=''");
    check_source_string(*s, L(""));

    s.reset(create(std::move(s), L("abc"), dummyLineLocation()));
    INFO("source='abc'");
    check_source_string(*s, L("abc"));

    s.reset(create(std::move(s), L("def\\\n"), dummyLineLocation()));
    INFO("source='abcdef\\\\\\n'");
    check_source_string(*s, L("abcdef\\\n"));

    s.reset(new source_stub(std::move(s), 6, 8, L("")));
    s.reset(create(std::move(s), L("ghi\n"), dummyLineLocation()));
    INFO("source='abcdefghi\\n'");
    check_source_string(*s, L("abcdefghi\n"));
}

TEST_CASE("Line-appended source line begin") {
    source::source_pointer s;

    s.reset(create(nullptr, L(""), dummyLineLocation()));
    INFO("source=''");
    check_source_line_begin(*s, {});

    s.reset(create(std::move(s), L("ab"), dummyLineLocation()));
    INFO("source='ab'");
    check_source_line_begin(*s, {});

    s.reset(create(std::move(s), L("cd\n"), dummyLineLocation()));
    INFO("source='abcd\\n'");
    check_source_line_begin(*s, {5});

    s.reset(create(std::move(s), L("e\\\n"), dummyLineLocation()));
    INFO("source='abcd\\ne\\\\\\n'");
    check_source_line_begin(*s, {5, 8});

    s.reset(new source_stub(std::move(s), 6, 8, L("")));
    s.reset(create(std::move(s), L(""), dummyLineLocation()));
    INFO("source='abcd\\ne'");
    check_source_line_begin(*s, {5});

    s.reset(create(std::move(s), L("\n"), dummyLineLocation()));
    INFO("source='abcd\\ne\\n'");
    check_source_line_begin(*s, {5, 7});
}

TEST_CASE("Line-appended source line end") {
    source::source_pointer s;

    s.reset(create(nullptr, L("ab"), dummyLineLocation()));
    INFO("source=''");
    check_source_line_end(*s, {2});

    s.reset(create(std::move(s), L("cd\n"), dummyLineLocation()));
    INFO("source='abcd\\n'");
    check_source_line_end(*s, {5});

    s.reset(create(std::move(s), L(""), dummyLineLocation()));
    check_source_line_end(*s, {5});

    s.reset(create(std::move(s), L("ef"), dummyLineLocation()));
    INFO("source='abcd\\nef'");
    check_source_line_end(*s, {5, 7});

    s.reset(create(std::move(s), L(""), dummyLineLocation()));
    check_source_line_end(*s, {5, 7});
}

TEST_CASE("Line-appended source location") {
    source::source_pointer s;

    s.reset(create(nullptr, L("ab"), dummyLineLocation(3)));
    check_source_location(*s, 0, 3, 0);
    check_source_location(*s, 1, 3, 1);

    s.reset(create(std::move(s), L("c\n"), dummyLineLocation(0)));
    check_source_location(*s, 0, 3, 0);
    check_source_location(*s, 1, 3, 1);
    check_source_location(*s, 2, 0, 0);
    check_source_location(*s, 3, 0, 1);

    s.reset(create(std::move(s), L("def"), dummyLineLocation(5)));
    check_source_location(*s, 0, 3, 0);
    check_source_location(*s, 1, 3, 1);
    check_source_location(*s, 2, 0, 0);
    check_source_location(*s, 3, 0, 1);
    check_source_location(*s, 4, 5, 0);
    check_source_location(*s, 5, 5, 1);
    check_source_location(*s, 6, 5, 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
