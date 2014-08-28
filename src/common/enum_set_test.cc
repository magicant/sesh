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

#include "common/enum_set.hh"

namespace {

using sesh::common::enum_set;
using sesh::common::enum_set_of;

enum class E : char { A, B, C, };

} // namespace

namespace sesh {
namespace common {

template<>
class EnumTraits<E> {
public:
    constexpr static E max = E::C;
}; // template<> struct EnumTraits<E>

} // namespace common
} // namespace sesh

namespace {

TEST_CASE("Enum set, construction") {
    const enum_set<E> s1{};
    enum_set<E> s2(s1);
    (void) s2;
}

TEST_CASE("Enum set, assignment") {
    const enum_set<E> s1{};
    enum_set<E> s2;
    s2 = s1;
}

TEST_CASE("Enum set, size") {
    static_assert(enum_set<E>::size() == 3, "Enum set, size");
}

TEST_CASE("Enum set, operator[]") {
    enum_set<E> s;
    s[E::B] = true;
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
    s[E::B] = false;
    CHECK_FALSE(s[E::B]);
}

TEST_CASE("Enum set, operator[] const") {
    const enum_set<E> s = enum_set<E>().set(E::B);
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, test") {
    const enum_set<E> s = enum_set<E>().set(E::B);
    CHECK_FALSE(s.test(E::A));
    CHECK(s.test(E::B));
    CHECK_FALSE(s.test(E::C));
}

TEST_CASE("Enum set, all") {
    const enum_set<E> all = enum_set<E>().set();
    const enum_set<E> not_all = enum_set<E>().set().reset(E::B);
    CHECK(all.all());
    CHECK_FALSE(not_all.all());
}

TEST_CASE("Enum set, any") {
    const enum_set<E> any = enum_set<E>().set(E::A);
    const enum_set<E> none = enum_set<E>();
    CHECK(any.any());
    CHECK_FALSE(none.any());
}

TEST_CASE("Enum set, none") {
    const enum_set<E> any = enum_set<E>().set(E::A);
    const enum_set<E> none = enum_set<E>();
    CHECK(none.none());
    CHECK_FALSE(any.none());
}

TEST_CASE("Enum set, count") {
    const enum_set<E> none = enum_set<E>();
    const enum_set<E> one = enum_set<E>(none).set(E::B);
    const enum_set<E> two = enum_set<E>(one).set(E::A);
    const enum_set<E> three = enum_set<E>(two).set(E::C);
    CHECK(none.count() == 0);
    CHECK(one.count() == 1);
    CHECK(two.count() == 2);
    CHECK(three.count() == 3);
}

TEST_CASE("Enum set, set") {
    enum_set<E> s;
    s.set();
    CHECK(s.all());
    s.set(E::B, false);
    CHECK_FALSE(s[E::B]);
    s.set(E::B);
    CHECK(s.all());
}

TEST_CASE("Enum set, reset") {
    enum_set<E> s;
    s.set();
    s.reset();
    CHECK(s.none());

    s.set();
    s.reset(E::C);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, flip") {
    enum_set<E> s;
    s.flip();
    CHECK(s.all());

    s.flip(E::A);
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK(s[E::C]);

    s.flip();
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, operator==") {
    enum_set<E> s1, s2;
    s1.set().reset(E::C);
    s2.set(E::A).set(E::B);
    CHECK(s1 == s2);
    CHECK_FALSE(s1 == enum_set<E>());
    CHECK_FALSE(s2 == enum_set<E>());
}

TEST_CASE("Enum set, operator!=") {
    enum_set<E> s1;
    s1.set(E::A);

    enum_set<E> s2 = s1;
    CHECK_FALSE(s1 != s2);

    s2.set(E::C);
    CHECK(s1 != s2);
}

TEST_CASE("Enum set, operator~") {
    const enum_set<E> s = enum_set<E>().set(E::A);
    CHECK_FALSE((~s)[E::A]);
    CHECK((~s)[E::B]);
    CHECK((~s)[E::C]);
}

TEST_CASE("Enum set, operator&") {
    const enum_set<E> s1 = enum_set<E>().set(E::A).set(E::B);
    const enum_set<E> s2 = enum_set<E>().set(E::B).set(E::C);
    const enum_set<E> s = s1 & s2;
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, operator|") {
    const enum_set<E> s1 = enum_set<E>().set(E::A);
    const enum_set<E> s2 = enum_set<E>().set(E::C);
    const enum_set<E> s = s1 | s2;
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator^") {
    const enum_set<E> s1 = enum_set<E>().set(E::A).set(E::B);
    const enum_set<E> s2 = enum_set<E>().set(E::B).set(E::C);
    const enum_set<E> s = s1 ^ s2;
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator&=") {
    enum_set<E> s = enum_set<E>().set(E::A).set(E::B);
    s &= enum_set<E>().set(E::B).set(E::C);
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, operator|=") {
    enum_set<E> s = enum_set<E>().set(E::A);
    s |= enum_set<E>().set(E::C);
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator^=") {
    enum_set<E> s = enum_set<E>().set(E::A).set(E::B);
    s ^= enum_set<E>().set(E::B).set(E::C);
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, enum set of") {
    CHECK(enum_set_of(E::A) == enum_set<E>().set(E::A));
    CHECK(enum_set_of(E::B) == enum_set<E>().set(E::B));
    CHECK(enum_set_of(E::C) == enum_set<E>().set(E::C));

    CHECK(enum_set_of(E::A, E::B) == enum_set<E>().set(E::A).set(E::B));
    CHECK(enum_set_of(E::A, E::B, E::C) ==
            enum_set<E>().set(E::A).set(E::B).set(E::C));
}

TEST_CASE("Enum set, to unsigned long") {
    CHECK(enum_set<E>().to_ulong() == 0UL);
    CHECK(enum_set_of(E::A).to_ulong() == 1UL << 0);
    CHECK(enum_set_of(E::B).to_ulong() == 1UL << 1);
    CHECK(enum_set_of(E::C).to_ulong() == 1UL << 2);
    CHECK(enum_set_of(E::A, E::B).to_ulong() == 3UL);
    CHECK(enum_set_of(E::A, E::C).to_ulong() == 5UL);
    CHECK(enum_set_of(E::B, E::C).to_ulong() == 6UL);
    CHECK(enum_set_of(E::A, E::B, E::C).to_ulong() == 7UL);
}

TEST_CASE("Enum set, to unsigned long long") {
    CHECK(enum_set<E>().to_ullong() == 0ULL);
    CHECK(enum_set_of(E::A).to_ullong() == 1ULL << 0);
    CHECK(enum_set_of(E::B).to_ullong() == 1ULL << 1);
    CHECK(enum_set_of(E::C).to_ullong() == 1ULL << 2);
    CHECK(enum_set_of(E::A, E::B).to_ullong() == 3ULL);
    CHECK(enum_set_of(E::A, E::C).to_ullong() == 5ULL);
    CHECK(enum_set_of(E::B, E::C).to_ullong() == 6ULL);
    CHECK(enum_set_of(E::A, E::B, E::C).to_ullong() == 7ULL);
}

TEST_CASE("Enum set, hash") {
    const enum_set<E> s1 = enum_set<E>().set(E::A).set(E::B);
    const enum_set<E> s2 = enum_set<E>().set().reset(E::C);
    std::hash<enum_set<E>> h;
    CHECK(h(s1) == h(s2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
