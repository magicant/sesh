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
#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironment.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Skipper.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::common::Char;
using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::language::parser::BasicEnvironment;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Skipper;
using sesh::language::source::Source;
using sesh::language::source::SourceStub;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

class EnvironmentStub : public BasicEnvironment {

    using BasicEnvironment::BasicEnvironment;

    bool mIsEof = false;

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

    void appendSource(String &&s) {
        substituteSource([&s](Source::Pointer &&orig) -> Source::Pointer {
            auto length = (orig == nullptr) ? 0 : orig->length();
            return Source::Pointer(new SourceStub(
                    std::move(orig), length, length, std::move(s)));
        });
    }

};

template<Char c>
bool is(Environment &, Char c2) {
    return c == c2;
}

TEST_CASE("Skipper construction") {
    EnvironmentStub e;
    Skipper s(e, is<L('\0')>);
    Skipper s2(s);
    Skipper(std::move(s2));
}

TEST_CASE("Skipper, 0 append") {
    EnvironmentStub e;
    Skipper s(e, is<L('$')>);

    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(String(L("AB\0C$XYZ"), 8));

    s.skip();
    CHECK(e.current() == e.begin() + 4);
}

TEST_CASE("Skipper, 1 append") {
    EnvironmentStub e;
    e.appendSource(L("ABC+XYZ"));
    e.current() += 4;

    Skipper s(e, is<L('-')>);

    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("$123-+"));

    s.skip();
    CHECK(e.current() == e.end() - 2);
}

TEST_CASE("Skipper, 0 append, to end") {
    EnvironmentStub e;
    Skipper s(e, is<L('x')>);
    e.appendSource(L("apple"));
    e.setIsEof();

    s.skip();
    CHECK(e.current() == e.end());
}

TEST_CASE("Skipper, 1 append, to end") {
    EnvironmentStub e;
    e.appendSource(L("hot do"));
    e.current() += 4;

    Skipper s(e, is<L('x')>);
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("g"));
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.setIsEof();

    s.skip();
    CHECK(e.current() == e.end());
}

TEST_CASE("Skipper, 1 append, empty result") {
    EnvironmentStub e;
    e.appendSource(L("X"));
    e.current() += 1;

    Skipper s(e, is<L('*')>);

    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("***"));

    s.skip();
    CHECK(e.current() == e.end() - 3);
}

TEST_CASE("Skipper, null stopper") {
    EnvironmentStub e;
    Skipper s(e, nullptr);

    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("Hello"));
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L(", "));
    e.appendSource(L("world"));
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("!"));
    e.setIsEof();

    s.skip();
    CHECK(e.current() == e.end());
}

TEST_CASE("Skipper, no remove line continuations 1") {
    EnvironmentStub e;
    Skipper s(e, is<L('@')>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("AB\\\nC@"));

    s.skip();
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("Skipper, no remove line continuations 2") {
    EnvironmentStub e;
    Skipper s(e, is<L('\\')>, LineContinuationTreatment::LITERAL);

    e.appendSource(L("AB\\\nC@"));

    s.skip();
    CHECK(e.current() == e.begin() + 2);
}

TEST_CASE("Skipper, remove line continuations 1") {
    EnvironmentStub e;
    Skipper s(e, is<L('@')>, LineContinuationTreatment::REMOVE);

    e.appendSource(L("ABC\\\\\n\nDEF\\"));
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.appendSource(L("\nGHI@"));

    s.skip();
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("Skipper, remove line continuations 2") {
    EnvironmentStub e;
    Skipper s(e, is<L('\\')>, LineContinuationTreatment::REMOVE);

    e.appendSource(L("ABC\\\nDEF\\"));
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.setIsEof();

    s.skip();
    CHECK(e.current() == e.end() - 1);
}

TEST_CASE("Skipper, reuse") {
    EnvironmentStub e;
    Skipper s(e, is<L('|')>);

    e.appendSource(L("-|--||----|"));

    s.skip();
    CHECK(e.current() == e.begin() + 1);
    s.skip();
    CHECK(e.current() == e.begin() + 1);

    e.current() += 1;
    s.skip();
    CHECK(e.current() == e.begin() + 4);
    s.skip();
    CHECK(e.current() == e.begin() + 4);

    e.current() += 1;
    s.skip();
    CHECK(e.current() == e.begin() + 5);
    s.skip();
    CHECK(e.current() == e.begin() + 5);

    e.current() += 2;
    s.skip();
    CHECK(e.current() == e.begin() + 10);
    s.skip();
    CHECK(e.current() == e.begin() + 10);

    e.current() += 1;
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);
    REQUIRE_THROWS_AS(s.skip(), NeedMoreSource);

    e.setIsEof();
    s.skip();
    CHECK(e.current() == e.begin() + 11);
    s.skip();
    CHECK(e.current() == e.begin() + 11);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
