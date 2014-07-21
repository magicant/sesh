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

#ifndef INCLUDED_common_EnumIterator_hh
#define INCLUDED_common_EnumIterator_hh

#include "buildconfig.h"

#include <limits>
#include <type_traits>
#include "boost/iterator/counting_iterator.hpp"
#include "boost/iterator/transform_iterator.hpp"
#include "common/EnumTraits.hh"
#include "common/StaticCast.hh"

namespace sesh {
namespace common {

namespace enum_iterator_impl {

template<typename E>
using Base = boost::transform_iterator<
        StaticCast<E, typename std::underlying_type<E>::type>,
        boost::counting_iterator<typename std::underlying_type<E>::type>>;

} // namespace enum_iterator_impl

/**
 * An enum iterator yields successive enum values by incrementing the
 * underlying integral value.
 */
template<typename E>
class EnumIterator : public enum_iterator_impl::Base<E> {

public:

    using Enum = E;
    using UnderlyingType = typename std::underlying_type<E>::type;

    explicit EnumIterator(UnderlyingType v) :
            enum_iterator_impl::Base<E>(
                    boost::make_counting_iterator(v),
                    StaticCast<E, UnderlyingType>()) { }

    EnumIterator(E e) : EnumIterator(static_cast<UnderlyingType>(e)) { }

}; // template<typename E> class EnumIterator

namespace enum_iterator_impl {

template<typename E>
class FullRange {

private:

    using U = typename EnumIterator<E>::UnderlyingType;

public:

    EnumIterator<E> begin() const {
        EnumIterator<E> i(static_cast<U>(0));
        return i;
    }

    EnumIterator<E> end() const {
        constexpr U max = static_cast<U>(EnumTraits<E>::max);
        static_assert(max < std::numeric_limits<U>::max(), "overflow");
        EnumIterator<E> i(max + 1);
        return i;
    }

}; // template<typename E> class FullRange

} // namespace enum_iterator_impl

/**
 * Returns a new object that has the begin and end member functions that are
 * suitable for ranged for-loops and that iterates all enumerators of an
 * enumeration type.
 *
 * @tparam E An enumeration type. Its first enumerator must have an underlying
 * value of zero. All enumerators must be defined successively (as an integer
 * sequence). The last enumerator must be specified by {@code
 * EnumTraits&lt;E>::max}.
 */
template<typename E>
enum_iterator_impl::FullRange<E> enumerators() noexcept {
    enum_iterator_impl::FullRange<E> range;
    return range;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_EnumIterator_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
