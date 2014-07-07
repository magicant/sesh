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

#ifndef INCLUDED_os_TimeApi_hh
#define INCLUDED_os_TimeApi_hh

#include "buildconfig.h"

#include <chrono>

namespace sesh {
namespace os {

/** Abstraction of POSIX API that provides current time. */
class TimeApi {

public:

    using SystemClockTime = std::chrono::time_point<
            std::chrono::system_clock, std::chrono::nanoseconds>;
    using SteadyClockTime = std::chrono::time_point<
            std::chrono::steady_clock, std::chrono::nanoseconds>;

    /** Returns the current time. */
    virtual SystemClockTime systemClockNow() const noexcept = 0;

    /** Returns the current time. */
    virtual SteadyClockTime steadyClockNow() const noexcept = 0;

}; // class TimeApi

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_TimeApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
