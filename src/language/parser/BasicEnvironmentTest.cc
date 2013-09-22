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

#include <stdexcept>
#include <utility>
#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::language::parser::BasicEnvironment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::source::Source;
using sesh::language::source::SourceBuffer;
using sesh::language::source::SourceStub;
using sesh::language::source::checkSourceString;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

class EnvironmentStub : public BasicEnvironment {

    bool mIsEof = false;

    using BasicEnvironment::BasicEnvironment;

    bool isEof() const noexcept override {
        return mIsEof;
    }

    bool substituteAlias(const Iterator &, const Iterator &) override {
        throw "unexpected";
    }

    void addDiagnosticMessage(const Iterator &, String &&, ErrorLevel)
            override {
        throw "unexpected";
    }

public:

    void setIsEof(bool isEof = true) {
        mIsEof = isEof;
    }

    Iterator begin() const {
        return sourceBuffer().begin();
    }

    void setSource(String &&s) {
        substituteSource([&s](Source::Pointer &&) -> Source::Pointer {
            return Source::Pointer(
                    new SourceStub(nullptr, 0, 0, std::move(s)));
        });
    }

    void checkSource(const String &string) {
        for (String::size_type i = 0; i < string.length(); ++i)
            CHECK(sourceBuffer().at(i) == string.at(i));
        CHECK_THROWS_AS(sourceBuffer().at(string.length()), std::out_of_range);
    }

};

TEST_CASE("Environment remove line continuation 1") {
    EnvironmentStub e;

    INFO("1 empty source");
    e.checkSource(L(""));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin()), NeedMoreSource);
    INFO("2 empty source");
    e.checkSource(L(""));
}

TEST_CASE("Environment remove line continuation 2") {
    EnvironmentStub e;

    e.setSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin()), NeedMoreSource);
    INFO("1 \\\\");
    e.checkSource(L("\\"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 1), NeedMoreSource);
    INFO("2 \\\\");
    e.checkSource(L("\\"));
}

TEST_CASE("Environment remove line continuation 3") {
    EnvironmentStub e;

    e.setSource(L("\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin()));
    INFO("1 \\n");
    e.checkSource(L("\n"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 1), NeedMoreSource);
    INFO("2 \\n");
    e.checkSource(L("\n"));
}

TEST_CASE("Environment remove line continuation 4") {
    EnvironmentStub e;

    e.setSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin()));
    INFO("1 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 3));
    INFO("2 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 4));
    INFO("3 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 5));
    INFO("4 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 7));
    INFO("5 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_FALSE(e.removeLineContinuation(e.begin() + 8));
    INFO("6 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK_THROWS_AS(e.removeLineContinuation(e.begin() + 9), NeedMoreSource);
    INFO("7 Test\\n\\\\\\\\\\n\\n");
    e.checkSource(L("Test\n\\\\\n\n"));

    CHECK(e.removeLineContinuation(e.begin() + 6));
    INFO("Test\\n\\\\\\n");
    e.checkSource(L("Test\n\\\n"));

    CHECK(e.removeLineContinuation(e.begin() + 5));
    INFO("Test\\n");
    e.checkSource(L("Test\n"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
