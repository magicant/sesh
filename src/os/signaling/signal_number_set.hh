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

#ifndef INCLUDED_os_signaling_signal_number_set_hh
#define INCLUDED_os_signaling_signal_number_set_hh

#include "buildconfig.h"

#include <memory>
#include "os/signaling/signal_number.hh"

namespace sesh {
namespace os {
namespace signaling {

/** A signal number set contains zero or more signal numbers. */
class signal_number_set {

public:

    signal_number_set() = default;
    signal_number_set(const signal_number_set &) = default;
    signal_number_set(signal_number_set &&) = default;
    signal_number_set &operator=(const signal_number_set &) = default;
    signal_number_set &operator=(signal_number_set &&) = default;
    virtual ~signal_number_set() = default;

    /** Checks if the given signal number is included in this set. */
    virtual bool test(signal_number) const = 0;

    /** Adds/removes a signal number to/from this set. */
    virtual signal_number_set &set(signal_number, bool = true) = 0;

    /** Removes a signal number from this set. */
    signal_number_set &reset(signal_number n) {
        return set(n, false);
    }

    /** Adds all signal numbers to this set. */
    virtual signal_number_set &set() = 0;

    /** Clears this set. */
    virtual signal_number_set &reset() = 0;

    /** Creates a copy of this instance. */
    virtual std::unique_ptr<signal_number_set> clone() const = 0;

    /** Reference to a single entry of a set. */
    class reference {

    private:

        signal_number_set &m_set;
        signal_number m_number;

    public:

        reference(signal_number_set &set, signal_number n) noexcept :
                m_set(set), m_number(n) { }

        reference &operator=(bool value) {
            m_set.set(m_number, value);
            return *this;
        }

        operator bool() {
            return m_set.test(m_number);
        }

        bool operator~() {
            return !*this;
        }

    }; // class reference

    /** Returns a reference to the specified entry of this set. */
    reference operator[](signal_number n) noexcept {
        return reference(*this, n);
    }

}; // class signal_number_set

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_signal_number_set_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
