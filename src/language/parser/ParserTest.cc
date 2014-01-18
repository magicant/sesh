/* Copyright (C) 2013-2014 WATANABE Yuki
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

#include "common/Maybe.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Reparse.hh"

namespace {

using sesh::common::Maybe;
using sesh::common::createMaybeOf;
using sesh::language::parser::IncompleteParse;
using sesh::language::parser::Parser;
using sesh::language::parser::ParserBase;
using sesh::language::parser::Reparse;
using sesh::language::parser::SourceTestEnvironment;

class CountingParserStub : public Parser<int> {

    using Parser::Parser;

public:

    unsigned mParseImplCallCount = 0;
    unsigned mResetImplCallCount = 0;

private:

    Maybe<int> mResult;

protected:

    Maybe<int> &result() noexcept override { return mResult; }

private:

    virtual void parseImpl2() = 0;
    virtual void resetImpl2() noexcept { }

    void parseImpl() final override {
        ++mParseImplCallCount;
        parseImpl2();
    }

    void resetImpl() noexcept final override {
        ++mResetImplCallCount;
        resetImpl2();
    }

}; // class CountingParserStub

class SucceedingParserStub : public CountingParserStub {

    using CountingParserStub::CountingParserStub;

    void parseImpl2() override {
        environment().setPosition(environment().length());
        result() = 123;
    }

}; // class SucceedingParserStub

class FailingParserStub : public CountingParserStub {

    using CountingParserStub::CountingParserStub;

    void parseImpl2() override {
        environment().setPosition(environment().length());
        result().clear();
    }

}; // class FailingParserStub

class ThrowingParserStub : public CountingParserStub {

    using CountingParserStub::CountingParserStub;

    void parseImpl2() override {
        throw IncompleteParse();
    }

}; // class ThrowingParserStub

TEST_CASE("Parser, environment") {
    SourceTestEnvironment e;
    ThrowingParserStub p(e);
    CHECK(&p.environment() == &e);
}

TEST_CASE("Parser, initial state") {
    SourceTestEnvironment e;
    ThrowingParserStub p(e);
    CHECK(p.state() == ParserBase::State::UNSTARTED);
    CHECK(p.mParseImplCallCount == 0);
    CHECK(p.mResetImplCallCount == 0);
}

TEST_CASE("Parser, reset from unstarted state") {
    SourceTestEnvironment e;
    ThrowingParserStub p(e);
    CHECK_NOTHROW(p.reset());
    CHECK(p.state() == ParserBase::State::UNSTARTED);
    CHECK(p.mParseImplCallCount == 0);
    CHECK(p.mResetImplCallCount == 1);
}

TEST_CASE("Parser, parse needs more source & reset from parsing state") {
    SourceTestEnvironment e;
    ThrowingParserStub p(e);

    CHECK_THROWS_AS(p.parse(), IncompleteParse);
    CHECK(p.state() == ParserBase::State::PARSING);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);

    CHECK_NOTHROW(p.reset());
    CHECK(p.state() == ParserBase::State::UNSTARTED);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 1);
}

TEST_CASE("Parser, successful parsing and idempotence") {
    SourceTestEnvironment e;
    SucceedingParserStub p(e);
    e.appendSource(L("ABC"));

    CHECK(p.parse() == createMaybeOf(123));
    CHECK(e.position() == e.length());
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);
    CHECK(p.state() == SucceedingParserStub::State::FINISHED);

    CHECK(p.parse() == createMaybeOf(123));
    CHECK(e.position() == e.length());
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);
    CHECK(p.state() == SucceedingParserStub::State::FINISHED);
}

TEST_CASE("Parser, unsuccessful parsing and idempotence") {
    SourceTestEnvironment e;
    FailingParserStub p(e);
    e.appendSource(L("ABC"));

    CHECK_FALSE(p.parse());
    CHECK(e.position() == 0);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);
    CHECK(p.state() == SucceedingParserStub::State::FINISHED);

    CHECK_FALSE(p.parse());
    CHECK(e.position() == 0);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);
    CHECK(p.state() == SucceedingParserStub::State::FINISHED);
}

TEST_CASE("Parser, reset from finished state") {
    SourceTestEnvironment e;
    SucceedingParserStub p(e);

    p.parse();
    CHECK(p.state() == ParserBase::State::FINISHED);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 0);

    CHECK_NOTHROW(p.reset());
    CHECK(p.state() == ParserBase::State::UNSTARTED);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 1);
}

TEST_CASE("Parser, reparse") {
    class ReparseParserStub : public CountingParserStub {
        using CountingParserStub::CountingParserStub;
        void parseImpl2() override {
            CHECK(mResetImplCallCount == 0);
            environment().setPosition(environment().length());
            throw Reparse();
        }
        void resetImpl2() noexcept override {
            CHECK(mParseImplCallCount == 1);
        }
    };

    SourceTestEnvironment e;
    ReparseParserStub p(e);
    e.appendSource(L("ABC"));

    CHECK_THROWS_AS(p.parse(), Reparse);
    CHECK(e.position() == 0);
    CHECK(p.state() == ParserBase::State::UNSTARTED);
    CHECK(p.mParseImplCallCount == 1);
    CHECK(p.mResetImplCallCount == 1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
