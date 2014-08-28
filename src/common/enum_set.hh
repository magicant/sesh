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

#ifndef INCLUDED_common_enum_set_hh
#define INCLUDED_common_enum_set_hh

#include "buildconfig.h"

#include <bitset>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include "common/enum_traits.hh"

namespace sesh {
namespace common {

/**
 * An enum set is a set of enumerators from a single enumerator type.
 *
 * This class template is a std::bitset wrapper that is more convenient for use
 * with enumerations, especially scoped ones.
 *
 * @tparam E An enumeration type. {@link enum_traits} must be specialized for
 * this enumeration.
 */
template<typename E>
class enum_set {

private:

    constexpr static std::size_t to_size(E e) noexcept {
        return static_cast<std::size_t>(e);
    }

    constexpr static std::size_t N = to_size(enum_traits<E>::max) + 1;

    static_assert(N != 0, "Too large enumeration");

    std::bitset<N> m_bitset;

    constexpr enum_set(std::bitset<N> s) noexcept : m_bitset(s) { }

public:

    using value_type = E;

    using reference = typename std::bitset<N>::reference;

    constexpr enum_set() = default;

    enum_set(std::initializer_list<E> values) : enum_set() {
        for (E value : values)
            set(value);
    }

    constexpr static std::size_t size() noexcept {
        return N;
    }

    constexpr bool operator[](E e) const {
        return m_bitset[to_size(e)];
    }
    reference operator[](E e) {
        return m_bitset[to_size(e)];
    }

    bool test(E e) const {
        return m_bitset.test(to_size(e));
    }

    bool all() const {
        return m_bitset.all();
    }
    bool any() const {
        return m_bitset.any();
    }
    bool none() const {
        return m_bitset.none();
    }

    std::size_t count() const {
        return m_bitset.count();
    }

    enum_set &set() noexcept {
        m_bitset.set();
        return *this;
    }
    enum_set &set(E e, bool value = true) {
        m_bitset.set(to_size(e), value);
        return *this;
    }
    enum_set &reset() noexcept {
        m_bitset.reset();
        return *this;
    }
    enum_set &reset(E e) {
        m_bitset.reset(to_size(e));
        return *this;
    }
    enum_set &flip() noexcept {
        m_bitset.flip();
        return *this;
    }
    enum_set &flip(E e) {
        m_bitset.flip(to_size(e));
        return *this;
    }

    bool operator==(const enum_set &s) const noexcept {
        return m_bitset == s.m_bitset;
    }
    bool operator!=(const enum_set &s) const noexcept {
        return m_bitset != s.m_bitset;
    }

    enum_set operator~() const noexcept {
        return enum_set(~m_bitset);
    }
    enum_set operator&(const enum_set &s) const noexcept {
        return enum_set(m_bitset & s.m_bitset);
    }
    enum_set operator|(const enum_set &s) const noexcept {
        return enum_set(m_bitset | s.m_bitset);
    }
    enum_set operator^(const enum_set &s) const noexcept {
        return enum_set(m_bitset ^ s.m_bitset);
    }

    enum_set &operator&=(const enum_set &s) {
        m_bitset &= s.m_bitset;
        return *this;
    }
    enum_set &operator|=(const enum_set &s) {
        m_bitset |= s.m_bitset;
        return *this;
    }
    enum_set &operator^=(const enum_set &s) {
        m_bitset ^= s.m_bitset;
        return *this;
    }

    unsigned long to_ulong() const {
        return m_bitset.to_ulong();
    }
    unsigned long long to_ullong() const {
        return m_bitset.to_ullong();
    }

    std::size_t hash() const noexcept {
        return std::hash<std::bitset<N>>()(m_bitset);
    }

}; // template<typename E> class enum_set

/** Creates a enum set that contains the given enumerators. */
template<typename E, typename... E2>
enum_set<E> enum_set_of(E head, E2... tail) {
    return enum_set<E>{ head, tail..., };
}

} // namespace common
} // namespace sesh

namespace std {

template<typename E>
struct hash<sesh::common::enum_set<E>> {

    std::size_t operator()(const sesh::common::enum_set<E> &s) const noexcept {
        return s.hash();
    }

}; // template<typename E> struct hash<sesh::common::enum_set<E>>

} // namespace std

#endif // #ifndef INCLUDED_common_enum_set_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
