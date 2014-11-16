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

#ifndef INCLUDED_common_constant_function_hh
#define INCLUDED_common_constant_function_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>

namespace sesh {
namespace common {

/**
 * A function object that always returns a reference to the same constant
 * object.
 *
 * @tparam T Type of the object that is contained in the function object and
 * returned by the function.
 */
template<typename T>
class constant_function {

public:

    using value_type = T;

private:

    value_type m_value;

public:

    /**
     * Constructs a constant function object by forwarding all arguments to the
     * constructor of the contained object.
     */
    template<
            typename... A,
            typename = typename std::enable_if<
                    std::is_constructible<value_type, A...>::value>::type>
    constexpr explicit constant_function(A &&... a)
            noexcept(std::is_nothrow_constructible<value_type, A...>::value) :
            m_value(std::forward<A>(a)...) { }

    /**
     * Returns a reference to the contained constant object ignoring all
     * arguments.
     */
    template<typename... A>
    constexpr const value_type &operator()(A &&...) const noexcept {
        return m_value;
    }

}; // template<typename T> class constant_function

/**
 * Returns a constant function that always returns a reference to a copy of the
 * argument.
 */
template<typename T>
auto constant(T &&v)
        noexcept(std::is_nothrow_constructible<
                constant_function<typename std::decay<T>::type>, T>::value)
        -> constant_function<typename std::decay<T>::type> {
    return constant_function<typename std::decay<T>::type>(std::forward<T>(v));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_constant_function_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
