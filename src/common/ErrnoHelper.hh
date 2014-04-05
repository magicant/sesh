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

#ifndef INCLUDED_common_ErrnoHelper_hh
#define INCLUDED_common_ErrnoHelper_hh

#include "buildconfig.h"

#include <cerrno>
#include <system_error>

namespace sesh {
namespace common {

/** Returns an error code object for the current errno value. */
inline std::error_code errnoCode() {
    return make_error_code(static_cast<std::errc>(errno));
}

/** Returns an error condition object for the current errno value. */
inline std::error_condition errnoCondition() {
    return make_error_condition(static_cast<std::errc>(errno));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_ErrnoHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
