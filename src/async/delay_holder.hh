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

#ifndef INCLUDED_async_delay_holder_hh
#define INCLUDED_async_delay_holder_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "async/delay.hh"

namespace sesh {
namespace async {

/** A non-copyable base class that has a shared pointer to a delay object. */
template<typename T>
class delay_holder {

private:

    std::shared_ptr<async::delay<T>> m_delay;

public:

    /**
     * The default constructor creates a delay holder without an associated
     * delay.
     */
    delay_holder() = default;

    /** Creates a delay holder that holds the argument delay. */
    explicit delay_holder(const std::shared_ptr<async::delay<T>> &d) noexcept :
            m_delay(d) { }

    delay_holder(const delay_holder &) = delete;
    delay_holder(delay_holder &&) = default;
    delay_holder &operator=(const delay_holder &) = delete;
    delay_holder &operator=(delay_holder &&) = default;
    ~delay_holder() = default;

    /** Checks if this delay holder has an associated delay object. */
    bool is_valid() const noexcept {
        return m_delay != nullptr;
    }

    /** Disconnects this delay holder from the associated delay object. */
    void invalidate() noexcept {
        m_delay.reset();
    }

protected:

    /**
     * Returns a reference to the associated delay. This function can be called
     * only when this object has an associated delay.
     */
    async::delay<T> &delay() { return *m_delay; }

    /**
     * Calls the forward function for the delay object contained in the
     * arguments.
     */
    static void forward(delay_holder &&from, delay_holder &&to) {
        async::delay<T>::forward(
                std::move(from.m_delay), std::move(to.m_delay));
    }

}; // template<typename T> class delay_holder

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_delay_holder_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
