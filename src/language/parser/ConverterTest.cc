/* Copyright (C) 2014 WATANABE Yuki
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

#include "language/parser/Converter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/FailingParser.hh"
#include "language/parser/NormalParser.hh"

namespace {

using sesh::language::parser::Converter;
using sesh::language::parser::Environment;
using sesh::language::parser::FailingParser;
using sesh::language::parser::NormalParser;
using sesh::language::parser::SourceTestEnvironment;

class ResetCounter : public NormalParser<int> {

private:

    int mCount;

public:

    ResetCounter(Environment &e, int count) noexcept :
            NormalParser<int>(e), mCount(count) { }

private:

    void parseImpl() override {
        result() = mCount;
    }

    void resetImpl() noexcept override {
        ++mCount;
        NormalParser<int>::resetImpl();
    }

}; // class ResetCounter

class ConverterStub : public Converter<ResetCounter, float> {

public:

    ConverterStub(Environment &e, int count) noexcept :
            Converter(e, e, count) { }

private:

    void convert(int &&i) override {
        result() = i * 10;
    }

}; // class ConverterStub

class NoopConverterStub : public Converter<FailingParser<int>, int> {

    using Converter<FailingParser<int>, int>::Converter;

    void convert(int &&) override {
        FAIL("unexpected convert");
    }

}; // class NoopConverterStub

class FailingConverterStub : public Converter<ResetCounter, int> {

public:

    explicit FailingConverterStub(Environment &e) noexcept :
            Converter(e, e, 0) { }

private:

    void convert(int &&) override { }

}; // class FailingConverterStub

TEST_CASE("Converter, failure of from-parser") {
    SourceTestEnvironment e;
    NoopConverterStub c(e, e);
    CHECK_FALSE(c.parse().hasValue());
}

TEST_CASE("Converter, conversion failure") {
    SourceTestEnvironment e;
    FailingConverterStub c(e);
    CHECK_FALSE(c.parse().hasValue());
}

TEST_CASE("Converter, successes and reset") {
    SourceTestEnvironment e;
    ConverterStub c(e, 3);

    REQUIRE(c.parse().hasValue());
    CHECK(c.parse().value() == 30.f);

    c.reset();

    REQUIRE(c.parse().hasValue());
    CHECK(c.parse().value() == 40.f);

    c.reset();
    c.reset();

    REQUIRE(c.parse().hasValue());
    CHECK(c.parse().value() == 60.f);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */