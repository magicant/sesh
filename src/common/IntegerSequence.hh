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

#ifndef INCLUDED_common_IntegerSequence_hh
#define INCLUDED_common_IntegerSequence_hh

#include "buildconfig.h"

#include <cstddef>

namespace sesh {
namespace common {

/**
 * Compile-time sequence of integers.
 *
 * @tparam Int type of integers
 * @tparam value values of the Int type
 */
template<typename Int, Int... value>
class IntegerSequence {

public:

    /** The type of values. */
    using value_type = Int;

    /** Returns the number of values in this sequence. */
    constexpr static std::size_t size() noexcept { return sizeof...(value); }

}; // template<typename Int, Int... value> class IntegerSequence

/**
 * Compile-time sequence of indices.
 *
 * @tparam value values of the std::size_t type.
 */
template<std::size_t... value>
using IndexSequence = IntegerSequence<std::size_t, value...>;

namespace integer_sequence_impl {

template<typename Int, Int tail, Int... head>
auto append(const IntegerSequence<Int, head...> &)
        -> IntegerSequence<Int, head..., tail>;

template<typename Int, Int n, bool isZero = n == 0>
class IntegerSequenceMaker;

template<typename Int, Int n>
class IntegerSequenceMaker<Int, n, true> {

public:

    using type = IntegerSequence<Int>;

};

template<typename Int, Int n>
class IntegerSequenceMaker<Int, n, false> {

private:

    static_assert(n >= 0, "An integer sequence must have a non-negative size");

    using Predecessor = typename IntegerSequenceMaker<Int, n - 1>::type;

public:

    using type = decltype(append<Int, n - 1>(Predecessor()));

};

} // namespace integer_sequence_impl

/**
 * Integer sequence of consecutive values from 0 to n-1.
 *
 * @tparam Int type of integers
 * @tparam n number of values
 */
template<typename Int, Int n>
using ConsecutiveIntegerSequence =
        typename integer_sequence_impl::IntegerSequenceMaker<Int, n>::type;

/**
 * Index sequence of consecutive values from 0 to n-1.
 *
 * @tparam n number of values
 */
template<std::size_t n>
using ConsecutiveIndexSequence = ConsecutiveIntegerSequence<std::size_t, n>;

/**
 * Consecutive index sequence of the same length as the template parameters.
 */
template<typename... T>
using IndexSequenceFor = ConsecutiveIndexSequence<sizeof...(T)>;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_IntegerSequence_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
