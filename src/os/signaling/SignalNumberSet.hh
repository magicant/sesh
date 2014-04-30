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

#ifndef INCLUDED_os_signaling_SignalNumberSet_hh
#define INCLUDED_os_signaling_SignalNumberSet_hh

#include "buildconfig.h"

#include <memory>
#include "os/signaling/SignalNumber.hh"

namespace sesh {
namespace os {
namespace signaling {

/** A signal number set contains zero or more signal numbers. */
class SignalNumberSet {

public:

    SignalNumberSet() = default;
    SignalNumberSet(const SignalNumberSet &) = default;
    SignalNumberSet(SignalNumberSet &&) = default;
    SignalNumberSet &operator=(const SignalNumberSet &) = default;
    SignalNumberSet &operator=(SignalNumberSet &&) = default;
    virtual ~SignalNumberSet() = default;

    /** Checks if the given signal number is included in this set. */
    virtual bool test(signaling::SignalNumber) const = 0;

    /** Adds/removes a signal number to/from this set. */
    virtual SignalNumberSet &set(signaling::SignalNumber, bool = true) = 0;

    /** Removes a signal number from this set. */
    SignalNumberSet &reset(signaling::SignalNumber n) {
        return set(n, false);
    }

    /** Adds all signal numbers to this set. */
    virtual SignalNumberSet &set() = 0;

    /** Clears this set. */
    virtual SignalNumberSet &reset() = 0;

    /** Creates a copy of this instance. */
    virtual std::unique_ptr<SignalNumberSet> clone() const = 0;

    /** Reference to a single entry of a set. */
    class Reference {

    private:

        SignalNumberSet &mSet;
        SignalNumber mNumber;

    public:

        Reference(SignalNumberSet &set, SignalNumber n) noexcept :
                mSet(set), mNumber(n) { }

        Reference &operator=(bool value) {
            mSet.set(mNumber, value);
            return *this;
        }

        operator bool() {
            return mSet.test(mNumber);
        }

        bool operator~() {
            return !*this;
        }

    }; // class Reference

    /** Returns a reference to the specified entry of this set. */
    Reference operator[](SignalNumber n) noexcept {
        return Reference(*this, n);
    }

}; // class SignalNumberSet

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_SignalNumberSet_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
