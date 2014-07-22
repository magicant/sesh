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

#include "common/EnumSet.hh"

namespace {

using sesh::common::EnumSet;
using sesh::common::enumSetOf;

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
    const EnumSet<E> s1{};
    EnumSet<E> s2(s1);
    (void) s2;
}

TEST_CASE("Enum set, assignment") {
    const EnumSet<E> s1{};
    EnumSet<E> s2;
    s2 = s1;
}

TEST_CASE("Enum set, size") {
    static_assert(EnumSet<E>::size() == 3, "Enum set, size");
}

TEST_CASE("Enum set, operator[]") {
    EnumSet<E> s;
    s[E::B] = true;
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
    s[E::B] = false;
    CHECK_FALSE(s[E::B]);
}

TEST_CASE("Enum set, operator[] const") {
    const EnumSet<E> s = EnumSet<E>().set(E::B);
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, test") {
    const EnumSet<E> s = EnumSet<E>().set(E::B);
    CHECK_FALSE(s.test(E::A));
    CHECK(s.test(E::B));
    CHECK_FALSE(s.test(E::C));
}

TEST_CASE("Enum set, all") {
    const EnumSet<E> all = EnumSet<E>().set();
    const EnumSet<E> notAll = EnumSet<E>().set().reset(E::B);
    CHECK(all.all());
    CHECK_FALSE(notAll.all());
}

TEST_CASE("Enum set, any") {
    const EnumSet<E> any = EnumSet<E>().set(E::A);
    const EnumSet<E> none = EnumSet<E>();
    CHECK(any.any());
    CHECK_FALSE(none.any());
}

TEST_CASE("Enum set, none") {
    const EnumSet<E> any = EnumSet<E>().set(E::A);
    const EnumSet<E> none = EnumSet<E>();
    CHECK(none.none());
    CHECK_FALSE(any.none());
}

TEST_CASE("Enum set, count") {
    const EnumSet<E> none = EnumSet<E>();
    const EnumSet<E> one = EnumSet<E>(none).set(E::B);
    const EnumSet<E> two = EnumSet<E>(one).set(E::A);
    const EnumSet<E> three = EnumSet<E>(two).set(E::C);
    CHECK(none.count() == 0);
    CHECK(one.count() == 1);
    CHECK(two.count() == 2);
    CHECK(three.count() == 3);
}

TEST_CASE("Enum set, set") {
    EnumSet<E> s;
    s.set();
    CHECK(s.all());
    s.set(E::B, false);
    CHECK_FALSE(s[E::B]);
    s.set(E::B);
    CHECK(s.all());
}

TEST_CASE("Enum set, reset") {
    EnumSet<E> s;
    s.set();
    s.reset();
    CHECK(s.none());

    s.set();
    s.reset(E::C);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, flip") {
    EnumSet<E> s;
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
    EnumSet<E> s1, s2;
    s1.set().reset(E::C);
    s2.set(E::A).set(E::B);
    CHECK(s1 == s2);
    CHECK_FALSE(s1 == EnumSet<E>());
    CHECK_FALSE(s2 == EnumSet<E>());
}

TEST_CASE("Enum set, operator!=") {
    EnumSet<E> s1;
    s1.set(E::A);

    EnumSet<E> s2 = s1;
    CHECK_FALSE(s1 != s2);

    s2.set(E::C);
    CHECK(s1 != s2);
}

TEST_CASE("Enum set, operator~") {
    const EnumSet<E> s = EnumSet<E>().set(E::A);
    CHECK_FALSE((~s)[E::A]);
    CHECK((~s)[E::B]);
    CHECK((~s)[E::C]);
}

TEST_CASE("Enum set, operator&") {
    const EnumSet<E> s1 = EnumSet<E>().set(E::A).set(E::B);
    const EnumSet<E> s2 = EnumSet<E>().set(E::B).set(E::C);
    const EnumSet<E> s = s1 & s2;
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, operator|") {
    const EnumSet<E> s1 = EnumSet<E>().set(E::A);
    const EnumSet<E> s2 = EnumSet<E>().set(E::C);
    const EnumSet<E> s = s1 | s2;
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator^") {
    const EnumSet<E> s1 = EnumSet<E>().set(E::A).set(E::B);
    const EnumSet<E> s2 = EnumSet<E>().set(E::B).set(E::C);
    const EnumSet<E> s = s1 ^ s2;
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator&=") {
    EnumSet<E> s = EnumSet<E>().set(E::A).set(E::B);
    s &= EnumSet<E>().set(E::B).set(E::C);
    CHECK_FALSE(s[E::A]);
    CHECK(s[E::B]);
    CHECK_FALSE(s[E::C]);
}

TEST_CASE("Enum set, operator|=") {
    EnumSet<E> s = EnumSet<E>().set(E::A);
    s |= EnumSet<E>().set(E::C);
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, operator^=") {
    EnumSet<E> s = EnumSet<E>().set(E::A).set(E::B);
    s ^= EnumSet<E>().set(E::B).set(E::C);
    CHECK(s[E::A]);
    CHECK_FALSE(s[E::B]);
    CHECK(s[E::C]);
}

TEST_CASE("Enum set, enum set of") {
    CHECK(enumSetOf(E::A) == EnumSet<E>().set(E::A));
    CHECK(enumSetOf(E::B) == EnumSet<E>().set(E::B));
    CHECK(enumSetOf(E::C) == EnumSet<E>().set(E::C));

    CHECK(enumSetOf(E::A, E::B) == EnumSet<E>().set(E::A).set(E::B));
    CHECK(enumSetOf(E::A, E::B, E::C) ==
            EnumSet<E>().set(E::A).set(E::B).set(E::C));
}

TEST_CASE("Enum set, to unsigned long") {
    CHECK(EnumSet<E>().to_ulong() == 0UL);
    CHECK(enumSetOf(E::A).to_ulong() == 1UL << 0);
    CHECK(enumSetOf(E::B).to_ulong() == 1UL << 1);
    CHECK(enumSetOf(E::C).to_ulong() == 1UL << 2);
    CHECK(enumSetOf(E::A, E::B).to_ulong() == 3UL);
    CHECK(enumSetOf(E::A, E::C).to_ulong() == 5UL);
    CHECK(enumSetOf(E::B, E::C).to_ulong() == 6UL);
    CHECK(enumSetOf(E::A, E::B, E::C).to_ulong() == 7UL);
}

TEST_CASE("Enum set, to unsigned long long") {
    CHECK(EnumSet<E>().to_ullong() == 0ULL);
    CHECK(enumSetOf(E::A).to_ullong() == 1ULL << 0);
    CHECK(enumSetOf(E::B).to_ullong() == 1ULL << 1);
    CHECK(enumSetOf(E::C).to_ullong() == 1ULL << 2);
    CHECK(enumSetOf(E::A, E::B).to_ullong() == 3ULL);
    CHECK(enumSetOf(E::A, E::C).to_ullong() == 5ULL);
    CHECK(enumSetOf(E::B, E::C).to_ullong() == 6ULL);
    CHECK(enumSetOf(E::A, E::B, E::C).to_ullong() == 7ULL);
}

TEST_CASE("Enum set, hash") {
    const EnumSet<E> s1 = EnumSet<E>().set(E::A).set(E::B);
    const EnumSet<E> s2 = EnumSet<E>().set().reset(E::C);
    std::hash<EnumSet<E>> h;
    CHECK(h(s1) == h(s2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
