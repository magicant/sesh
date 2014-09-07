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

#ifndef INCLUDED_os_event_timeout_hh
#define INCLUDED_os_event_timeout_hh

#include "buildconfig.h"

#include <chrono>

namespace sesh {
namespace os {
namespace event {

/**
 * A timeout event trigger.
 *
 * @see Trigger
 */
class timeout {

public:

    using internal_type = std::chrono::nanoseconds;

private:

    internal_type m_interval;

public:

    constexpr explicit timeout(internal_type i) noexcept : m_interval(i) { }

    constexpr internal_type interval() const noexcept { return m_interval; }

};

constexpr inline bool operator==(const timeout &l, const timeout &r) noexcept {
    return l.interval() == r.interval();
}

constexpr inline bool operator<(const timeout &l, const timeout &r) noexcept {
    return l.interval() < r.interval();
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_timeout_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
