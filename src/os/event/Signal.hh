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

#ifndef INCLUDED_os_event_Signal_hh
#define INCLUDED_os_event_Signal_hh

#include "buildconfig.h"

#include "os/signaling/SignalNumber.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * A signal event trigger.
 *
 * @see Trigger
 */
class Signal {

private:

    signaling::SignalNumber mNumber;

public:

    constexpr explicit Signal(signaling::SignalNumber n) noexcept :
            mNumber(n) { }

    constexpr signaling::SignalNumber number() const noexcept {
        return mNumber;
    }

}; // class Signal

constexpr inline bool operator==(const Signal &l, const Signal &r) noexcept {
    return l.number() == r.number();
}

constexpr inline bool operator<(const Signal &l, const Signal &r) noexcept {
    return l.number() < r.number();
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_Signal_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
