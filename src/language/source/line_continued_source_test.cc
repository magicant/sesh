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

#include <stdexcept>
#include "catch.hpp"
#include "common/xchar.hh"
#include "language/source/line_continued_source.hh"
#include "language/source/source_test_helper.hh"

namespace {

using sesh::language::source::line_continued_source;
using sesh::language::source::source;
using sesh::language::source::source_stub;

TEST_CASE("Line-continued source construction") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("\\\n")));
    CHECK_NOTHROW(s.reset(new line_continued_source(std::move(s), 0)));

    s.reset(new source_stub(nullptr, 0, 0, L("abc\\\ndef")));
    CHECK_NOTHROW(s.reset(new line_continued_source(std::move(s), 3)));

    s.reset(new source_stub(nullptr, 0, 0, L("\\\\\n\n")));
    CHECK_NOTHROW(s.reset(new line_continued_source(std::move(s), 1)));
}

TEST_CASE("Line-continued source construction exception") {
    source::source_pointer s;

    CHECK_THROWS_AS(line_continued_source(nullptr, 0), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\\")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 0), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\\")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 1), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\n")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 0), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\n")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 1), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(
            line_continued_source(std::move(s), 0),
            std::invalid_argument);

    s.reset(new source_stub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 1), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 2), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(
            line_continued_source(std::move(s), 0),
            std::invalid_argument);

    s.reset(new source_stub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 1), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(line_continued_source(std::move(s), 2), std::out_of_range);

    s.reset(new source_stub(nullptr, 0, 0, L("\\a")));
    CHECK_THROWS_AS(
            line_continued_source(std::move(s), 0),
            std::invalid_argument);

    s.reset(new source_stub(nullptr, 0, 0, L("a\n")));
    CHECK_THROWS_AS(
            line_continued_source(std::move(s), 0),
            std::invalid_argument);

    s.reset(new source_stub(nullptr, 0, 0, L("ab")));
    CHECK_THROWS_AS(
            line_continued_source(std::move(s), 0),
            std::invalid_argument);
}

TEST_CASE("Line-continued source assignment") {
    line_continued_source lcs(
            source::source_pointer(new source_stub(nullptr, 0, 0, L("\\\n"))),
            0);
    CHECK_NOTHROW(lcs = line_continued_source(
            source::source_pointer(new source_stub(nullptr, 0, 0, L("\\\n"))),
            0));
}

TEST_CASE("Line-continued source value") {
    source::source_pointer s;

    s.reset(new source_stub(nullptr, 0, 0, L("\\\n")));
    s.reset(new line_continued_source(std::move(s), 0));
    INFO("source='' (1)");
    check_source_string(*s, L(""));

    s.reset(new source_stub(nullptr, 0, 0, L("abc\\\ndef")));
    s.reset(new line_continued_source(std::move(s), 3));
    INFO("source='abcdef'");
    check_source_string(*s, L("abcdef"));

    s.reset(new source_stub(nullptr, 0, 0, L("\\\\\n\n")));
    s.reset(new line_continued_source(std::move(s), 1));
    INFO("source='\\\\\\n'");
    check_source_string(*s, L("\\\n"));

    s.reset(new line_continued_source(std::move(s), 0));
    INFO("source='' (2)");
    check_source_string(*s, L(""));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
