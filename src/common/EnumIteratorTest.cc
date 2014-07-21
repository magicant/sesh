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

#include <type_traits>
#include <vector>
#include "common/EnumIterator.hh"
#include "common/EnumTraits.hh"

namespace {

using sesh::common::EnumIterator;
using sesh::common::enumerators;

enum A { A0, A1, A2, A3, };
enum class B { B3 = 3, B4, B5, };

enum class C { C0, C1, C2, };

}

namespace sesh {
namespace common {

template<>
class EnumTraits<C> {
public:
    constexpr static C max = C::C2;
};

} // namespace common
} // namespace sesh

namespace {

TEST_CASE("Enum iterator: basic iteration on unscoped enumeration") {
    EnumIterator<A> i = A1;
    using UnderlyingType = decltype(i)::UnderlyingType;

    CHECK(*i == A1);
    ++i;
    CHECK(*i == A2);
    i += 2;
    CHECK(i == EnumIterator<A>(static_cast<UnderlyingType>(4)));
    --i;
    CHECK(*i == A3);
    i--;
    CHECK(*i == A2);
    i -= 2;
    CHECK(*i == A0);
    i++;
    CHECK(*i == A1);
}

TEST_CASE("Enum iterator: basic iteration on scoped enumeration") {
    EnumIterator<B> i = B::B3;
    CHECK(*i == B::B3);
    ++i;
    CHECK(*i == B::B4);
    i += 2;
    CHECK(i == EnumIterator<B>(6));
    --i;
    CHECK(*i == B::B5);
}

TEST_CASE("Enum iterator: direct construction from underlying type") {
    EnumIterator<B> i(3);
    CHECK(*i == B::B3);
}

TEST_CASE("Enum iterator: enumerators of unscoped enumeration") {
    std::vector<C> cs;
    for (auto e : enumerators<C>()) {
        static_assert(
                std::is_same<decltype(e), C>::value,
                "Iterated value should be of the enumeration type.");
        cs.push_back(e);
    }
    CHECK(cs == (std::vector<C>{C::C0, C::C1, C::C2}));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
