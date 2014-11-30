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

#include "async/lazy.hh"
#include "catch.hpp"
#include "common/empty.hh"

namespace {

using sesh::async::lazy;
using sesh::async::lazy_error;
using sesh::common::empty;

struct move_only {
    move_only() = default;
    move_only(move_only &&) = default;
    move_only &operator=(move_only &&) = default;
};

TEST_CASE("Lazy default constructor uses value default constructor") {
    lazy<empty> l;
    CHECK(*l);
    CHECK(&*l == &*l);
}

TEST_CASE("Lazy default constructor uses value default constructor (const)") {
    const lazy<empty> l;
    CHECK(*l);
    CHECK(&*l == &*l);
}

TEST_CASE("Lazy value computation") {
    bool called = false;
    lazy<int> l([&called] { CHECK_FALSE(called); called = true; return 42; });
    CHECK(l->get() == 42);
    CHECK(called);
    CHECK(l->get() == 42);
}

TEST_CASE("Lazy::has_computed") {
    lazy<empty> l([&l] { CHECK_FALSE(l.has_computed()); return empty(); });
    CHECK_FALSE(l.has_computed());
    (void) *l;
    CHECK(l.has_computed());
}

TEST_CASE("Lazy::is_computing") {
    lazy<empty> l([&l] { CHECK(l.is_computing()); return empty(); });
    CHECK_FALSE(l.is_computing());
    (void) *l;
    CHECK_FALSE(l.is_computing());
}

TEST_CASE("Lazy: copy construction from already computed value") {
    const empty e{};
    lazy<empty> l = e;
    CHECK_FALSE(l.is_computing());
    CHECK(l.has_computed());
}

TEST_CASE("Lazy: move construction from already computed value") {
    lazy<move_only> l = move_only();
    CHECK_FALSE(l.is_computing());
    CHECK(l.has_computed());
}

TEST_CASE("Lazy with non-default-constructible value") {
    struct no_default {
        explicit no_default(int) { }
    };
    lazy<no_default> l([] { return no_default(0); });
    CHECK_NOTHROW(*l);
}

TEST_CASE("Lazy: -> operator") {
    struct X {
        int method() { return 7; }
    };
    lazy<X> l;
    CHECK(l->get().method() == 7);
}

TEST_CASE("Lazy: -> operator (const)") {
    struct X {
        int method() const { return 7; }
    };
    const lazy<X> l;
    CHECK(l->get().method() == 7);
}

TEST_CASE("Exception while computing lazy value") {
    struct E { };
    lazy<empty> l([]() -> empty { throw E(); });
    CHECK_FALSE(*l);
    CHECK_THROWS_AS(l->get(), E);
}

TEST_CASE("Recursive value computation results in lazy_error") {
    lazy<empty> l([&l] { CHECK_THROWS_AS(*l, lazy_error); return empty(); });
    CHECK_NOTHROW(*l);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
