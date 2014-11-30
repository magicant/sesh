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

#ifndef INCLUDED_common_enum_iterator_hh
#define INCLUDED_common_enum_iterator_hh

#include "buildconfig.h"

#include <limits>
#include <type_traits>
#include "boost/iterator/counting_iterator.hpp"
#include "boost/iterator/transform_iterator.hpp"
#include "common/enum_traits.hh"
#include "common/static_cast_function.hh"

namespace sesh {
namespace common {

namespace enum_iterator_impl {

template<typename E>
using base = boost::transform_iterator<
        static_cast_function<E, typename std::underlying_type<E>::type>,
        boost::counting_iterator<typename std::underlying_type<E>::type>>;

} // namespace enum_iterator_impl

/**
 * An enum iterator yields successive enum values by incrementing the
 * underlying integral value.
 */
template<typename E>
class enum_iterator : public enum_iterator_impl::base<E> {

public:

    using underlying_type = typename std::underlying_type<E>::type;

    explicit enum_iterator(underlying_type v) :
            enum_iterator_impl::base<E>(
                    boost::make_counting_iterator(v),
                    static_cast_function<E, underlying_type>()) { }

    enum_iterator(E e) : enum_iterator(static_cast<underlying_type>(e)) { }

}; // template<typename E> class enum_iterator

namespace enum_iterator_impl {

template<typename E>
class full_range {

private:

    using U = typename enum_iterator<E>::underlying_type;

public:

    enum_iterator<E> begin() const {
        enum_iterator<E> i(static_cast<U>(0));
        return i;
    }

    enum_iterator<E> end() const {
        constexpr U max = static_cast<U>(enum_traits<E>::max);
        static_assert(max < std::numeric_limits<U>::max(), "overflow");
        enum_iterator<E> i(max + 1);
        return i;
    }

}; // template<typename E> class full_range

} // namespace enum_iterator_impl

/**
 * Returns a new object that has the begin and end member functions that are
 * suitable for ranged for-loops and that iterates all enumerators of an
 * enumeration type.
 *
 * @tparam E An enumeration type. Its first enumerator must have an underlying
 * value of zero. All enumerators must be defined successively (as an integer
 * sequence). The last enumerator must be specified by
 * <code>enum_traits&lt;E>::max</code>.
 */
template<typename E>
enum_iterator_impl::full_range<E> enumerators() noexcept {
    enum_iterator_impl::full_range<E> range;
    return range;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_enum_iterator_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
