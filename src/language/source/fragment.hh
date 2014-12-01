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

#ifndef INCLUDED_language_source_fragment_hh
#define INCLUDED_language_source_fragment_hh

#include "buildconfig.h"

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace source {

// Defined just below.
class fragment;

/**
 * A fragment position is a pair of a pointer to a fragment and unsigned
 * integral index. It denotes a specific character in the fragment.
 *
 * A fragment position is a forward iterator. It is dereferenceable and
 * incrementable if and only if its {@link #head} is non-null.
 */
class fragment_position :
        public std::iterator<
                std::forward_iterator_tag,
                common::xstring::value_type,
                common::xstring::difference_type,
                common::xstring::const_pointer,
                common::xstring::const_reference> {

public:

    using string_type = common::xstring;
    using value_type = string_type::value_type;
    using size_type = string_type::size_type;

    /**
     * Index into the value of the fragment. The index must be less than the
     * length of the {@link #head}'s value. If {@link #head} is null, the index
     * must be zero.
     */
    size_type index = 0;

    /** Nullable pointer to a non-empty fragment. */
    std::shared_ptr<const fragment> head;
    // "head" has to be declared after "index" so that the implicitly-defined
    // assignment operator assigns "index" before assigning "head" to avoid
    // possible bad pointer access.

    fragment_position() = default;

    template<
            typename Head,
            typename = typename std::enable_if<
                    std::is_constructible<decltype(head), Head>::value>::type>
    fragment_position(Head &&head, size_type index = 0)
            noexcept(std::is_nothrow_constructible<
                    std::shared_ptr<const fragment>, Head>::value) :
            index(index), head(std::forward<Head>(head)) { }

}; // class fragment_position

/**
 * Represents a fragment of shell script source code.
 *
 * A fragment instance has a string that is the actual value of the source code
 * fragment. It also has a fragment_position that may point to another fragment
 * that follows this fragment in the source code.
 */
class fragment {

public:

    using value_type = fragment_position::string_type;

    /** Source code fragment value. */
    value_type value;
    /**
     * Nullable pointer to another fragment that follows the value of this
     * fragment in the source code.
     */
    fragment_position rest;

    template<
            typename V = value_type,
            typename R = std::nullptr_t,
            typename = typename std::enable_if<
                    std::is_constructible<decltype(value), V>::value>::type,
            typename = typename std::enable_if<
                    std::is_constructible<decltype(rest), R>::value>::type>
    fragment(V &&v = value_type(), R &&r = nullptr) :
            value(std::forward<V>(v)), rest(std::forward<R>(r)) { }

}; // class fragment

inline fragment_position::reference operator*(const fragment_position &p) {
    return p.head->value[p.index];
}

fragment_position &operator++(fragment_position &p);

inline fragment_position operator++(fragment_position &p, int) {
    fragment_position p2 = p;
    ++p;
    return p2;
}

inline bool operator==(
        const fragment_position &p1, const fragment_position &p2) {
    return p1.head == p2.head && p1.index == p2.index;
}

inline bool operator!=(
        const fragment_position &p1, const fragment_position &p2) {
    return !(p1 == p2);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_fragment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
