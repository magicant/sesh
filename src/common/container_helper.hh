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

#ifndef INCLUDED_common_container_helper_hh
#define INCLUDED_common_container_helper_hh

#include "buildconfig.h"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace sesh {
namespace common {

namespace find_impl {

/** Dummy function for SFINAE. */
template<typename Container, typename Element>
auto find(const Container &c, const Element &e) -> decltype(c.find(e));

/** Dummy function for SFINAE. */
void find(...);

} // namespace find_impl

/**
 * Searches the given container for an element equal to the given object.
 *
 * This overload is for containers that support the find non-static member
 * function. Note that the multi-set and multi-map's find function does not
 * always return the first of applicable elements.
 *
 * @return An iterator pointing to the found element, or a past-the-end
 * iterator if not found.
 */
template<typename Container, typename Element>
auto find(const Container &c, const Element &e)
        -> typename std::enable_if<
                std::is_same<
                    decltype(find_impl::find(c, e)),
                    typename Container::const_iterator>::value,
                typename Container::const_iterator>::type {
    return c.find(e);
}

/**
 * Searches the given container for an element equal to the given object.
 *
 * This overload is for containers that don't support the find non-static
 * member function.
 *
 * @return An iterator pointing to the first found element, or a past-the-end
 * iterator if not found.
 */
template<typename Container, typename Element>
auto find(const Container &c, const Element &e)
        -> typename std::enable_if<
                !std::is_same<
                    decltype(find_impl::find(c, e)),
                    typename Container::const_iterator>::value,
                typename Container::const_iterator>::type {
    return std::find(std::begin(c), std::end(c), e);
}

/**
 * Searches the given container for an element that meets the given predicate.
 *
 * @return An iterator pointing to the first found element, or a past-the-end
 * iterator if not found.
 */
template<typename Container, typename UnaryPredicate>
auto find_if(Container &c, UnaryPredicate &&p) -> decltype(std::begin(c)) {
    return std::find_if(
            std::begin(c), std::end(c), std::forward<UnaryPredicate>(p));
}

/**
 * Searches the given container for an element that does not meet the given
 * predicate.
 *
 * @return An iterator pointing to the first found element, or a past-the-end
 * iterator if not found.
 */
template<typename Container, typename UnaryPredicate>
auto find_if_not(Container &c, UnaryPredicate &&p) -> decltype(std::begin(c)) {
    return std::find_if_not(
            std::begin(c), std::end(c), std::forward<UnaryPredicate>(p));
}

/** Checks if the given range contains the given element. */
template<typename InputIterator, typename Element>
bool contains(InputIterator begin, InputIterator end, const Element &e) {
    return std::find(begin, end, e) != end;
}

/** Checks if the given container contains the given element. */
template<typename Container, typename Element>
bool contains(const Container &c, const Element &e) {
    return find(c, e) != c.end();
}

/**
 * Creates a new vector that contains the arguments. The vector elements are
 * move- or copy-constructed from the arguments.
 */
template<typename T, typename... Arg>
std::vector<T> make_vector_of(Arg &&... arg) {
    std::vector<T> v;
    v.reserve(sizeof...(arg));

    /*
     * List-initialization guarantees sequential evaluation of the parameter
     * pack expansion. XXX To work around the GCC 4.8 bug, we use a dummy array
     * here.
     */
    // Nop{(v.push_back(std::forward<Arg>(arg)), 0)...};
    int dummy[] {(v.push_back(std::forward<Arg>(arg)), 0)..., 0};
    (void) dummy;

    return v;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_container_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
