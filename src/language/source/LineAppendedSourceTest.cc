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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <utility>
#include "common/Char.hh"
#include "language/source/LineAppendedSource.hh"
#include "language/source/SourceLineLocationTestHelper.hh"
#include "language/source/SourceTestHelper.hh"

using sesh::language::source::LineAppendedSource;
using sesh::language::source::Source;
using sesh::language::source::SourceLineLocation;
using sesh::language::source::SourceLocation;
using sesh::language::source::SourceStub;
using sesh::language::source::checkSourceLineBegin;
using sesh::language::source::checkSourceLineEnd;
using sesh::language::source::checkSourceLocation;
using sesh::language::source::checkSourceString;
using sesh::language::source::dummySourceLineLocation;

using Pointer = sesh::language::source::LineAppendedSource::Pointer;
using String = sesh::language::source::LineAppendedSource::String;

namespace {

LineAppendedSource *create(
        Pointer &&original, String &&line, SourceLineLocation &&location) {
    return new LineAppendedSource(LineAppendedSource::create(
                std::move(original), std::move(line), std::move(location)));
}

} // namespace

TEST_CASE("Line-appended source construction no throw") {
    Source::Pointer s;

    CHECK_NOTHROW(s.reset(create(
            nullptr, L(""), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("\n"), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("test\n"), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("more"), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("line"), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("last\n"), dummySourceLineLocation())));

    CHECK_NOTHROW(s.reset(create(
            nullptr, L("txt\n"), dummySourceLineLocation())));
    CHECK_NOTHROW(s.reset(new SourceStub(std::move(s), 1, 2, L("es"))));
    CHECK_NOTHROW(s.reset(create(
            std::move(s), L("next\n"), dummySourceLineLocation())));
}

TEST_CASE("Line-appended source construction throw") {
    Source::Pointer s;

    CHECK_THROWS_AS(
            s.reset(create(nullptr, L("t\next"), dummySourceLineLocation())),
            std::invalid_argument);

    CHECK_THROWS_AS(
            s.reset(create(nullptr, L("\n\n"), dummySourceLineLocation())),
            std::invalid_argument);
}

TEST_CASE("Line-appended source assignment") {
    LineAppendedSource las = LineAppendedSource::create(
            nullptr, L(""), dummySourceLineLocation());
    las = LineAppendedSource::create(
            nullptr, L(""), dummySourceLineLocation());
}

TEST_CASE("Line-appended source value") {
    Source::Pointer s;

    s.reset(create(nullptr, L(""), dummySourceLineLocation()));
    INFO("source=''");
    checkSourceString(*s, L(""));

    s.reset(create(std::move(s), L("abc"), dummySourceLineLocation()));
    INFO("source='abc'");
    checkSourceString(*s, L("abc"));

    s.reset(create(std::move(s), L("def\\\n"), dummySourceLineLocation()));
    INFO("source='abcdef\\\\\\n'");
    checkSourceString(*s, L("abcdef\\\n"));

    s.reset(new SourceStub(std::move(s), 6, 8, L("")));
    s.reset(create(std::move(s), L("ghi\n"), dummySourceLineLocation()));
    INFO("source='abcdefghi\\n'");
    checkSourceString(*s, L("abcdefghi\n"));
}

TEST_CASE("Line-appended source line begin") {
    Source::Pointer s;

    s.reset(create(nullptr, L(""), dummySourceLineLocation()));
    INFO("source=''");
    checkSourceLineBegin(*s, {});

    s.reset(create(std::move(s), L("ab"), dummySourceLineLocation()));
    INFO("source='ab'");
    checkSourceLineBegin(*s, {});

    s.reset(create(std::move(s), L("cd\n"), dummySourceLineLocation()));
    INFO("source='abcd\\n'");
    checkSourceLineBegin(*s, {5});

    s.reset(create(std::move(s), L("e\\\n"), dummySourceLineLocation()));
    INFO("source='abcd\\ne\\\\\\n'");
    checkSourceLineBegin(*s, {5, 8});

    s.reset(new SourceStub(std::move(s), 6, 8, L("")));
    s.reset(create(std::move(s), L(""), dummySourceLineLocation()));
    INFO("source='abcd\\ne'");
    checkSourceLineBegin(*s, {5});

    s.reset(create(std::move(s), L("\n"), dummySourceLineLocation()));
    INFO("source='abcd\\ne\\n'");
    checkSourceLineBegin(*s, {5, 7});
}

TEST_CASE("Line-appended source line end") {
    Source::Pointer s;

    s.reset(create(nullptr, L("ab"), dummySourceLineLocation()));
    INFO("source=''");
    checkSourceLineEnd(*s, {2});

    s.reset(create(std::move(s), L("cd\n"), dummySourceLineLocation()));
    INFO("source='abcd\\n'");
    checkSourceLineEnd(*s, {5});

    s.reset(create(std::move(s), L(""), dummySourceLineLocation()));
    checkSourceLineEnd(*s, {5});

    s.reset(create(std::move(s), L("ef"), dummySourceLineLocation()));
    INFO("source='abcd\\nef'");
    checkSourceLineEnd(*s, {5, 7});

    s.reset(create(std::move(s), L(""), dummySourceLineLocation()));
    checkSourceLineEnd(*s, {5, 7});
}

TEST_CASE("Line-appended source location") {
    Source::Pointer s;
    auto dsll = dummySourceLineLocation;

    s.reset(create(nullptr, L("ab"), dsll(L("source 1"), 3)));
    checkSourceLocation(*s, 0, L("source 1"), 3, 0);
    checkSourceLocation(*s, 1, L("source 1"), 3, 1);

    s.reset(create(std::move(s), L("c\n"), dsll(L("source 2"), 0)));
    checkSourceLocation(*s, 0, L("source 1"), 3, 0);
    checkSourceLocation(*s, 1, L("source 1"), 3, 1);
    checkSourceLocation(*s, 2, L("source 2"), 0, 0);
    checkSourceLocation(*s, 3, L("source 2"), 0, 1);

    s.reset(create(std::move(s), L("def"), dsll(L("source 3"), 5)));
    checkSourceLocation(*s, 0, L("source 1"), 3, 0);
    checkSourceLocation(*s, 1, L("source 1"), 3, 1);
    checkSourceLocation(*s, 2, L("source 2"), 0, 0);
    checkSourceLocation(*s, 3, L("source 2"), 0, 1);
    checkSourceLocation(*s, 4, L("source 3"), 5, 0);
    checkSourceLocation(*s, 5, L("source 3"), 5, 1);
    checkSourceLocation(*s, 6, L("source 3"), 5, 2);
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
