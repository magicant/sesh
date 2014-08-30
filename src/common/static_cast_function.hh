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

#ifndef INCLUDED_common_static_cast_function_hh
#define INCLUDED_common_static_cast_function_hh

#include "buildconfig.h"

#include <utility>

namespace sesh {
namespace common {

/**
 * A function object that performs a static cast.
 * @tparam To The type to which the value is cast.
 * @tparam From The type from which the value is cast. If void, this object
 * doesn't care the type.
 */
template<typename To, typename From = void>
class static_cast_function {

public:

    using Result = To;

    constexpr To operator()(From v) const
            noexcept(noexcept(static_cast<To>(v))) {
        return static_cast<To>(v);
    }

}; // template<typename To, typename From> class static_cast_function

/**
 * This specialization of the static cast class template tries to static-cast
 * the argument value regardless of its type.
 */
template<typename To>
class static_cast_function<To, void> {

public:

    using Result = To;

    template<typename From>
    constexpr To operator()(From &&v) const
            noexcept(noexcept(static_cast<To>(std::move(v)))) {
        return static_cast<To>(std::move(v));
    }

}; // template<typename To> class static_cast_function<To, void>

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_static_cast_function_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
