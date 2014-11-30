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

#ifndef INCLUDED_common_copy_hh
#define INCLUDED_common_copy_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>

namespace sesh {
namespace common {

/** Returns a copy of the argument. */
template<typename T, typename R = typename std::decay<T>::type>
constexpr R copy(T &&t)
        noexcept(std::is_nothrow_constructible<R, T &&>::value &&
                std::is_nothrow_destructible<R>::value) {
    return std::forward<T>(t);
}

/**
 * Returns a copy of the argument unless the argument is an r-value reference,
 * in which case the reference is returned intact.
 */
template<
        typename T,
        typename R = typename std::decay<T>::type,
        typename S = typename std::conditional<
                std::is_rvalue_reference<T &&>::value, T &&, R>::type>
constexpr S copy_or_move(T &&t)
        noexcept(std::is_rvalue_reference<T &&>::value ||
                (std::is_nothrow_constructible<R, T &&>::value &&
                std::is_nothrow_destructible<R>::value)) {
    return std::forward<T>(t);
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_copy_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
