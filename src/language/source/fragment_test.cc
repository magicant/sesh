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

#include <memory>
#include <type_traits>
#include "catch.hpp"
#include "common/xchar.hh"
#include "language/source/fragment.hh"

namespace {

using sesh::language::source::fragment;
using sesh::language::source::fragment_position;

struct nonnull_position {
    std::shared_ptr<fragment> f{std::make_shared<fragment>()};
    fragment_position p{f};
    nonnull_position() {
        f->value = L("test");
    }
};

TEST_CASE("Fragment position is iterator") {
    CHECK(std::is_copy_constructible<fragment_position>::value);
    CHECK(std::is_copy_assignable<fragment_position>::value);
    CHECK(std::is_nothrow_destructible<fragment_position>::value);

    // forward iterator requirement
    CHECK(std::is_default_constructible<fragment_position>::value);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position is swappable") {
    fragment_position p2;
    p.index = 2;

    {
        using std::swap;
        swap(p, p2);
    }
    CHECK(p.head == nullptr);
    CHECK(p.index == 0);
    CHECK(p2.head == f);
    CHECK(p2.index == 2);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position dereference") {
    CHECK(*p == f->value[0]);

    p.index = 3;
    CHECK(*p == f->value[3]);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position simple increment") {
    ++p;
    CHECK(p.head == f);
    CHECK(p.index == 1);

    ++p;
    CHECK(p.head == f);
    CHECK(p.index == 2);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position increment to end") {
    p.index = p.head->value.size() - 1;
    ++p;
    CHECK(p.head == nullptr);
    CHECK(p.index == 0);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position increment to rest") {
    p.head = std::make_shared<fragment>(L("x"), p);
    ++p;
    CHECK(p.head == f);
    CHECK(p.index == 0);
}

// iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position increment to 2nd rest") {
    p.head = std::make_shared<fragment>(
            L("x"), std::make_shared<fragment>(L(""), p));
    ++p;
    CHECK(p.head == f);
    CHECK(p.index == 0);
}

// input iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position is equatable") {
    auto p2 = p;
    CHECK(p == p2);
    CHECK(p != fragment_position());
}

// input iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position post-increment") {
    auto p2 = p;
    CHECK(*p++ == *p2);
    CHECK(p == ++p2);
}

// forward iterator requirement
TEST_CASE_METHOD(nonnull_position, "Fragment position is reusable") {
    auto p1 = p, p2 = p;
    CHECK(++p1 == ++p2);
}

// forward iterator requirement
TEST_CASE("Value-initialized fragment positions are equal") {
    fragment_position p1{}, p2{};
    CHECK(p1 == p2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
