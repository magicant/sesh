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

#ifndef INCLUDED_os_event_signal_hh
#define INCLUDED_os_event_signal_hh

#include "buildconfig.h"

#include "os/signaling/signal_number.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * A signal event trigger.
 *
 * @see trigger
 */
class signal {

private:

    signaling::signal_number m_number;

public:

    constexpr explicit signal(signaling::signal_number n) noexcept :
            m_number(n) { }

    constexpr signaling::signal_number number() const noexcept {
        return m_number;
    }

}; // class signal

constexpr inline bool operator==(const signal &l, const signal &r) noexcept {
    return l.number() == r.number();
}

constexpr inline bool operator<(const signal &l, const signal &r) noexcept {
    return l.number() < r.number();
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_signal_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
