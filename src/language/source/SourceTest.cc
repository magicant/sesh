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

#include <memory>
#include <stdexcept>
#include "language/source/Source.hh"
#include "language/source/SourceTestHelper.hh"

using sesh::language::source::Source;
using sesh::language::source::SourceLocation;
using sesh::language::source::SourceStub;
using sesh::language::source::checkSourceLineBegin;
using sesh::language::source::checkSourceLineEnd;
using sesh::language::source::checkSourceString;

TEST_CASE("Source construction") {
    Source::Pointer s;

    CHECK_NOTHROW(s.reset(new SourceStub(nullptr, 0, 0, L"")));
    CHECK_NOTHROW(s.reset(new SourceStub(std::move(s), 0, 0, L"test")));
    CHECK_NOTHROW(s.reset(new SourceStub(std::move(s), 0, 4, L"more test")));
    CHECK_NOTHROW(s.reset(new SourceStub(std::move(s), 0, 9, L"")));
}

TEST_CASE("Source assignment") {
    Source::Pointer s(new SourceStub(nullptr, 0, 0, L""));
    SourceStub ss(std::move(s), 0, 0, L"");
    CHECK_NOTHROW(ss = SourceStub(nullptr, 0, 0, L""));
}

TEST_CASE("Source construction exception") {
    Source::Pointer s;

    CHECK_THROWS_AS(SourceStub(nullptr, 1, 0, L"test"), std::out_of_range);
    CHECK_THROWS_AS(SourceStub(nullptr, 0, 1, L"test"), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L"test"));
    CHECK_THROWS_AS(SourceStub(std::move(s), 3, 2, L""), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L"test"));
    CHECK_THROWS_AS(SourceStub(std::move(s), 0, 5, L""), std::out_of_range);

    s.reset(new SourceStub(nullptr, 0, 0, L"test"));
    CHECK_THROWS_AS(SourceStub(std::move(s), 5, 0, L""), std::out_of_range);
}

TEST_CASE("Source length") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L""));
    CHECK(s->length() == 0);

    s.reset(new SourceStub(nullptr, 0, 0, L"test"));
    CHECK(s->length() == 4);

    s.reset(new SourceStub(std::move(s), 1, 3, L"test"));
    CHECK(s->length() == 6);

    s.reset(new SourceStub(std::move(s), 0, 6, L""));
    CHECK(s->length() == 0);
}

TEST_CASE("Source at and operator[]") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L""));
    INFO("source=''");
    checkSourceString(*s, L"");

    s.reset(new SourceStub(std::move(s), 0, 0, L"x"));
    INFO("source='x'");
    checkSourceString(*s, L"x");

    s.reset(new SourceStub(std::move(s), 0, 0, L"t"));
    INFO("source='tx'");
    checkSourceString(*s, L"tx");

    s.reset(new SourceStub(std::move(s), 2, 2, L"t"));
    INFO("source='txt'");
    checkSourceString(*s, L"txt");

    s.reset(new SourceStub(std::move(s), 1, 2, L"es"));
    INFO("source='test'");
    checkSourceString(*s, L"test");

    s.reset(new SourceStub(std::move(s), 1, 3, L""));
    INFO("source='tt'");
    checkSourceString(*s, L"tt");
}

TEST_CASE("Source line begin") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L"abc\nxyz\n"));
    INFO("source='abc\\nxyz\\n'");
    checkSourceLineBegin(*s, {4, 8});

    s.reset(new SourceStub(std::move(s), 3, 4, L"d\n\nw"));
    INFO("source='abcd\\n\\nwxyz\\n'");
    checkSourceLineBegin(*s, {5, 6, 11});

    s.reset(new SourceStub(nullptr, 0, 0, L"\n\n"));
    INFO("source='\\n\\n'");
    checkSourceLineBegin(*s, {1, 2});

    s.reset(new SourceStub(std::move(s), 1, 1, L"\n\n"));
    INFO("source='\\n\\n\\n\\n'");
    checkSourceLineBegin(*s, {1, 2, 3, 4});

    s.reset(new SourceStub(std::move(s), 1, 3, L""));
    INFO("source='\\n\\n'");
    checkSourceLineBegin(*s, {1, 2});

    s.reset(new SourceStub(nullptr, 0, 0, L"abc\nxyz"));
    s.reset(new SourceStub(std::move(s), 2, 5, L"p"));
    INFO("source='abpyz'");
    checkSourceLineBegin(*s, {});
}

TEST_CASE("Source line end") {
    Source::Pointer s;

    s.reset(new SourceStub(nullptr, 0, 0, L"abc\nxyz\n"));
    INFO("source='abc\\nxyz\\n'");
    checkSourceLineEnd(*s, {4, 8});

    // L"abcd\n\nwxyz\n"
    s.reset(new SourceStub(std::move(s), 3, 4, L"d\n\nw"));
    INFO("source='abcd\\n\\nwxyz\\n'");
    checkSourceLineEnd(*s, {5, 6, 11});

    s.reset(new SourceStub(nullptr, 0, 0, L"\n\n"));
    INFO("source='\\n\\n'");
    checkSourceLineEnd(*s, {1, 2});

    s.reset(new SourceStub(std::move(s), 1, 1, L"\n\n"));
    INFO("source='\\n\\n\\n\\n'");
    checkSourceLineEnd(*s, {1, 2, 3, 4});

    s.reset(new SourceStub(std::move(s), 1, 3, L""));
    INFO("source='\\n\\n'");
    checkSourceLineEnd(*s, {1, 2});

    s.reset(new SourceStub(nullptr, 0, 0, L"abc\nxyz"));
    s.reset(new SourceStub(std::move(s), 2, 5, L"p"));
    INFO("source='abpyz'");
    checkSourceLineEnd(*s, {5});

    s.reset(new SourceStub(std::move(s), 5, 5, L"\n"));
    INFO("source='abpyz\\n'");
    checkSourceLineEnd(*s, {6});
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
