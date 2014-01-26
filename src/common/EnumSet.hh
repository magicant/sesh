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

#ifndef INCLUDED_common_EnumSet_hh
#define INCLUDED_common_EnumSet_hh

#include "buildconfig.h"

#include <bitset>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include "common/EnumTraits.hh"

namespace sesh {
namespace common {

/**
 * An enum set is a set of enumerators from a single enumerator type.
 *
 * This class template is a std::bitset wrapper that is more convenient for use
 * with enumerations, especially scoped ones.
 *
 * @tparam E An enumeration type. {@link EnumTraits} must be specialized for
 * this enumeration.
 */
template<typename E>
class EnumSet {

private:

    constexpr static std::size_t toSize(E e) noexcept {
        return static_cast<std::size_t>(e);
    }

    constexpr static std::size_t N = toSize(EnumTraits<E>::max) + 1;

    static_assert(N != 0, "Too large enumeration");

    std::bitset<N> mBitset;

    constexpr EnumSet(std::bitset<N> s) noexcept : mBitset(s) { }

public:

    using Enum = E;

    using reference = typename std::bitset<N>::reference;

    constexpr EnumSet() = default;

    EnumSet(std::initializer_list<E> values) : EnumSet() {
        for (E value : values)
            set(value);
    }

    constexpr static std::size_t size() noexcept {
        return N;
    }

    constexpr bool operator[](E e) const {
        return mBitset[toSize(e)];
    }
    reference operator[](E e) {
        return mBitset[toSize(e)];
    }

    bool test(E e) const {
        return mBitset.test(toSize(e));
    }

    bool all() const {
        return mBitset.all();
    }
    bool any() const {
        return mBitset.any();
    }
    bool none() const {
        return mBitset.none();
    }

    std::size_t count() const {
        return mBitset.count();
    }

    EnumSet &set() noexcept {
        mBitset.set();
        return *this;
    }
    EnumSet &set(E e, bool value = true) {
        mBitset.set(toSize(e), value);
        return *this;
    }
    EnumSet &reset() noexcept {
        mBitset.reset();
        return *this;
    }
    EnumSet &reset(E e) {
        mBitset.reset(toSize(e));
        return *this;
    }
    EnumSet &flip() noexcept {
        mBitset.flip();
        return *this;
    }
    EnumSet &flip(E e) {
        mBitset.flip(toSize(e));
        return *this;
    }

    bool operator==(const EnumSet &s) const noexcept {
        return mBitset == s.mBitset;
    }
    bool operator!=(const EnumSet &s) const noexcept {
        return mBitset != s.mBitset;
    }

    EnumSet operator~() const noexcept {
        return EnumSet(~mBitset);
    }
    EnumSet operator&(const EnumSet &s) const noexcept {
        return EnumSet(mBitset & s.mBitset);
    }
    EnumSet operator|(const EnumSet &s) const noexcept {
        return EnumSet(mBitset | s.mBitset);
    }
    EnumSet operator^(const EnumSet &s) const noexcept {
        return EnumSet(mBitset ^ s.mBitset);
    }

    EnumSet &operator&=(const EnumSet &s) {
        mBitset &= s.mBitset;
        return *this;
    }
    EnumSet &operator|=(const EnumSet &s) {
        mBitset |= s.mBitset;
        return *this;
    }
    EnumSet &operator^=(const EnumSet &s) {
        mBitset ^= s.mBitset;
        return *this;
    }

    std::size_t hash() const noexcept {
        return std::hash<std::bitset<N>>()(mBitset);
    }

}; // template<typename E> class EnumSet

/** Creates a enum set that contains the given enumerators. */
template<typename E, typename... E2>
EnumSet<E> enumSetOf(E head, E2... tail) {
    return EnumSet<E>{ head, tail..., };
}

} // namespace common
} // namespace sesh

namespace std {

template<typename E>
struct hash<sesh::common::EnumSet<E>> {

    std::size_t operator()(const sesh::common::EnumSet<E> &s) const noexcept {
        return s.hash();
    }

}; // template<typename E> struct hash<sesh::common::EnumSet<E>>

} // namespace std

#endif // #ifndef INCLUDED_common_EnumSet_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
