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

#ifndef INCLUDED_common_identity_hh
#define INCLUDED_common_identity_hh

#include "buildconfig.h"

namespace sesh {
namespace common {

/** An empty function object that implements an identity function. */
class identity {

public:

    /**
     * Just returns the argument. Note that the argument and return value are
     * passed by reference.
     */
    template<typename T>
    constexpr T &&operator()(T &&t) const volatile noexcept {
        return static_cast<T &&>(t); // std::forward<T>(t);
    }

}; // class identity

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_identity_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
