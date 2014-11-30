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

#include "catch.hpp"
#include "common/type_tag_set.hh"

namespace {

using sesh::common::type_tag;
using sesh::common::type_tag_set;

class A { };
class B { };

TEST_CASE("Type tag set default constructor makes empty set") {
    type_tag_set<A, B> s;
    CHECK(s.none());
}

TEST_CASE("Type tag set list initialization") {
    const type_tag_set<A, B> s = {type_tag<B>()};
    CHECK_FALSE(s[type_tag<A>()]);
    CHECK(s[type_tag<B>()]);
}

TEST_CASE("Adding type tag to type tag set") {
    type_tag_set<A, B> s;
    s.set(type_tag<A>());
    CHECK(s[type_tag<A>()]);
    CHECK_FALSE(s[type_tag<B>()]);
}

TEST_CASE("Removing type tag to type tag set") {
    type_tag_set<A, B> s;
    s.set(type_tag<A>());
    s.set(type_tag<A>(), false);
    CHECK(s.none());
}

TEST_CASE("Type tag set copy construction and equality") {
    const type_tag_set<A, B> s = {type_tag<B>()};
    type_tag_set<A, B> s2 = s;
    CHECK(s2 == s);
    CHECK_FALSE(s2 != s);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
