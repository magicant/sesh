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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <stdexcept>
#include "common/Char.hh"
#include "language/source/LineContinuedSource.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::language::source::LineContinuedSource;
using sesh::language::source::Source;
using sesh::language::source::SourceStub;

TEST_CASE("Line-continued source construction") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\n")));
    CHECK_NOTHROW(s.reset(new LineContinuedSource(std::move(s), 0)));

    s.reset(new SourceStub(nullptr, 0, 0, L("abc\\\ndef")));
    CHECK_NOTHROW(s.reset(new LineContinuedSource(std::move(s), 3)));

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\\\n\n")));
    CHECK_NOTHROW(s.reset(new LineContinuedSource(std::move(s), 1)));
}

TEST_CASE("Line-continued source construction exception") {
    Source::Pointer s;

    CHECK_THROWS_AS(LineContinuedSource(nullptr, 0), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 0), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 1), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\n")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 0), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\n")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 1), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(
            LineContinuedSource(std::move(s), 0),
            std::invalid_argument);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 1), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\\")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 2), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(
            LineContinuedSource(std::move(s), 0),
            std::invalid_argument);

    s.reset(new SourceStub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 1), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\n\n")));
    CHECK_THROWS_AS(LineContinuedSource(std::move(s), 2), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L("\\a")));
    CHECK_THROWS_AS(
            LineContinuedSource(std::move(s), 0),
            std::invalid_argument);

    s.reset(new SourceStub(nullptr, 0, 0, L("a\n")));
    CHECK_THROWS_AS(
            LineContinuedSource(std::move(s), 0),
            std::invalid_argument);

    s.reset(new SourceStub(nullptr, 0, 0, L("ab")));
    CHECK_THROWS_AS(
            LineContinuedSource(std::move(s), 0),
            std::invalid_argument);
}

TEST_CASE("Line-continued source assignment") {
    LineContinuedSource lcs(
            Source::Pointer(new SourceStub(nullptr, 0, 0, L("\\\n"))),
            0);
    CHECK_NOTHROW(lcs = LineContinuedSource(
            Source::Pointer(new SourceStub(nullptr, 0, 0, L("\\\n"))),
            0));
}

TEST_CASE("Line-continued source value") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\n")));
    s.reset(new LineContinuedSource(std::move(s), 0));
    INFO("source='' (1)");
    checkSourceString(*s, L(""));

    s.reset(new SourceStub(nullptr, 0, 0, L("abc\\\ndef")));
    s.reset(new LineContinuedSource(std::move(s), 3));
    INFO("source='abcdef'");
    checkSourceString(*s, L("abcdef"));

    s.reset(new SourceStub(nullptr, 0, 0, L("\\\\\n\n")));
    s.reset(new LineContinuedSource(std::move(s), 1));
    INFO("source='\\\\\\n'");
    checkSourceString(*s, L("\\\n"));

    s.reset(new LineContinuedSource(std::move(s), 0));
    INFO("source='' (2)");
    checkSourceString(*s, L(""));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
