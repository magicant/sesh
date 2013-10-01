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

#include <locale>
#include <utility>
#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::common::CharTraits;
using sesh::common::ErrorLevel;
using sesh::common::String;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Environment;
using sesh::language::source::Source;
using sesh::language::source::SourceBuffer;
using sesh::language::source::SourceStub;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

class EmptyEnvironment : public Environment {

public:

    ~EmptyEnvironment() override = default;

private:

    Iterator &current() noexcept override {
        throw "unexpected";
    }

    const Iterator &end() const noexcept override {
        throw "unexpected";
    }

    bool isEof() const noexcept override {
        throw "unexpected";
    }

    bool removeLineContinuation(const Iterator &) override {
        throw "unexpected";
    }

    bool substituteAlias(const Iterator &, const Iterator &) override {
        throw "unexpected";
    }

    void addDiagnosticMessage(const Iterator &, String &&, ErrorLevel)
            override {
        throw "unexpected";
    }

    const std::locale &locale() const override {
        throw "unexpected";
    }

};

class DereferenceTestStubEnvironment : public Environment {

private:

    SourceBuffer::Pointer mBuffer;
    Iterator mCurrent;
    Iterator mEnd;
    bool mIsEof;

public:

    DereferenceTestStubEnvironment(String &&s, bool isEof) {
        mBuffer = SourceBuffer::create();
        mBuffer->substitute([&s](Source::Pointer &&) -> Source::Pointer {
            return Source::Pointer(
                    new SourceStub(nullptr, 0, 0, std::move(s)));
        });
        mCurrent = mBuffer->begin();
        mEnd = mBuffer->end();
        mIsEof = isEof;
    }

    ~DereferenceTestStubEnvironment() override = default;

    Iterator &current() noexcept {
        return mCurrent;
    }

    const Iterator &end() const noexcept override {
        return mEnd;
    }

    bool isEof() const noexcept override {
        return mIsEof;
    }

    void testDereference(const String &s) {
        Iterator i = current();

        for (auto j = s.cbegin(); j != s.cend(); ++i, ++j)
            CHECK(dereference(*this, i) == CharTraits::to_int_type(*j));
        if (isEof())
            CHECK(CharTraits::eq_int_type(
                        dereference(*this, i),
                        CharTraits::eof()));
        else
            CHECK_THROWS_AS(dereference(*this, i), NeedMoreSource);
    }

private:

    bool removeLineContinuation(const Iterator &) override {
        throw "unexpected";
    }

    bool substituteAlias(const Iterator &, const Iterator &) override {
        throw "unexpected";
    }

    void addDiagnosticMessage(const Iterator &, String &&, ErrorLevel)
            override {
        throw "unexpected";
    }

    const std::locale &locale() const override {
        throw "unexpected";
    }

};

TEST_CASE("Environment construction and assignment") {
    EmptyEnvironment e1;
    EmptyEnvironment e2(e1);
    EmptyEnvironment e3(std::move(e2));

    e2 = e3;
    e2 = std::move(e3);
}

TEST_CASE("Environment dereference eof") {
    String s = L("Test");
    DereferenceTestStubEnvironment(String(s), true).testDereference(s);
}

TEST_CASE("Environment dereference no-eof") {
    String s = L("String");
    DereferenceTestStubEnvironment(String(s), false).testDereference(s);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
