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

#include <iterator>
#include "catch.hpp"
#include "common/either.hh"
#include "common/xchar.hh"
#include "language/source/stream.hh"

namespace {

using sesh::common::trial;
using sesh::language::source::empty_stream;
using sesh::language::source::fragment;
using sesh::language::source::fragment_position;
using sesh::language::source::stream;
using sesh::language::source::stream_of;
using sesh::language::source::stream_value;

void check_empty_stream(const stream &s) {
    bool called = false;
    s->get().then([&called](const trial<stream_value> &t) {
        REQUIRE(t);
        CHECK(t->first == nullptr);
        called = true;
    });
    CHECK(called);
}

TEST_CASE("Empty stream") {
    check_empty_stream(empty_stream());
}

TEST_CASE("Stream of empty fragment") {
    check_empty_stream(stream_of(fragment_position()));
}

TEST_CASE("Stream of empty stream") {
    check_empty_stream(stream_of(fragment_position(), empty_stream()));
}

TEST_CASE("Stream of fragment chain") {
    const fragment_position fp1(
            std::make_shared<const fragment>(L("34")));
    const fragment_position fp2(
            std::make_shared<const fragment>(L("..12"), fp1), 2);
    auto s = stream_of(fp2);
    bool called = false;
    s->get().then([&](const trial<stream_value> &t) {
        REQUIRE(t);
        CHECK(t->first == fp2);
        t->second->get().then([&](const trial<stream_value> &t) {
            REQUIRE(t);
            CHECK(t->first == std::next(fp2));
            t->second->get().then([&](const trial<stream_value> &t) {
                REQUIRE(t);
                CHECK(t->first == fp1);
                t->second->get().then([&](const trial<stream_value> &t) {
                    REQUIRE(t);
                    CHECK(t->first == std::next(fp1));
                    t->second->get().then([&](const trial<stream_value> &t) {
                        REQUIRE(t);
                        CHECK(t->first == nullptr);
                        called = true;
                    });
                });
            });
        });
    });
    CHECK(called);
}

TEST_CASE("Prepending empty fragment") {
    const fragment_position fp(std::make_shared<const fragment>(L("1")));
    auto s = stream_of(fragment_position(), stream_of(fp));
    bool called = false;
    s->get().then([&](const trial<stream_value> &t) {
        REQUIRE(t);
        CHECK(t->first == fp);
        t->second->get().then([&](const trial<stream_value> &t) {
            REQUIRE(t);
            CHECK(t->first == nullptr);
            called = true;
        });
    });
    CHECK(called);
}

TEST_CASE("Stream chain") {
    const fragment_position fp1(std::make_shared<const fragment>(L("1")));
    const fragment_position fp2(std::make_shared<const fragment>(L("2")));
    auto s = stream_of(fp1, stream_of(fp2));
    bool called = false;
    s->get().then([&](const trial<stream_value> &t) {
        REQUIRE(t);
        CHECK(t->first == fp1);
        t->second->get().then([&](const trial<stream_value> &t) {
            REQUIRE(t);
            CHECK(t->first == fp2);
            t->second->get().then([&](const trial<stream_value> &t) {
                REQUIRE(t);
                CHECK(t->first == nullptr);
                called = true;
            });
        });
    });
    CHECK(called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
