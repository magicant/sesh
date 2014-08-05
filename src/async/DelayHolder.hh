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

#ifndef INCLUDED_async_DelayHolder_hh
#define INCLUDED_async_DelayHolder_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "async/Delay.hh"

namespace sesh {
namespace async {

/** A non-copyable base class that has a shared pointer to a delay object. */
template<typename T>
class DelayHolder {

private:

    std::shared_ptr<Delay<T>> mDelay;

public:

    /**
     * The default constructor creates a delay holder without an associated
     * delay.
     */
    DelayHolder() = default;

    /** Creates a delay holder that holds the argument delay. */
    explicit DelayHolder(const std::shared_ptr<Delay<T>> &delay) noexcept :
            mDelay(delay) { }

    DelayHolder(const DelayHolder &) = delete;
    DelayHolder(DelayHolder &&) = default;
    DelayHolder &operator=(const DelayHolder &) = delete;
    DelayHolder &operator=(DelayHolder &&) = default;
    ~DelayHolder() = default;

    /** Checks if this delay holder has an associated delay object. */
    bool isValid() const noexcept {
        return mDelay != nullptr;
    }

    /** Disconnects this delay holder from the associated delay object. */
    void invalidate() noexcept {
        mDelay.reset();
    }

protected:

    /**
     * Returns a reference to the associated delay. This function can be called
     * only when this object has an associated delay.
     */
    Delay<T> &delay() { return *mDelay; }

    /**
     * Calls the forward function for the delay object contained in the
     * arguments.
     */
    static void forward(DelayHolder &&from, DelayHolder &&to) {
        Delay<T>::forward(std::move(from.mDelay), std::move(to.mDelay));
    }

}; // template<typename T> class DelayHolder

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_DelayHolder_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
