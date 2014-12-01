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

#ifndef INCLUDED_os_event_user_provided_trigger_hh
#define INCLUDED_os_event_user_provided_trigger_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "async/future.hh"
#include "common/variant.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * A user-provided trigger can be used to trigger an event manually.
 *
 * A user-provided trigger is constructed from a future that provides a shared
 * pointer to void. The event is triggered by setting a shared pointer to the
 * promise corresponding to the future. The event handler can access the shared
 * pointer via the {@link #result()} method.
 */
class user_provided_trigger {

public:

    using result_type = std::shared_ptr<void>;

private:

    common::variant<async::future<result_type>, result_type> m_value;

public:

    /** Constructs a new user-provided trigger from the given future. */
    explicit user_provided_trigger(async::future<result_type> &&f) noexcept :
            m_value(std::move(f)) { }

    /**
     * Constructs a new user-provided trigger that has the given result.
     *
     * User-provided triggers constructed by this constructor must not be used
     * to specify a trigger set.
     */
    explicit user_provided_trigger(result_type &&r) noexcept :
            m_value(std::move(r)) { }

    /**
     * Returns a reference to the future that triggers the event. This function
     * can be called only for objects that were constructed by the constructor
     * that takes a future argument.
     */
    async::future<result_type> &future() noexcept {
        return m_value.value<async::future<result_type>>();
    }

    /**
     * Returns a reference to the result shared pointer. This function can be
     * called only for objects that were constructed by the constructor that
     * takes a shared pointer argument.
     */
    result_type &result() noexcept {
        return m_value.value<result_type>();
    }
    /**
     * Returns a reference to the result shared pointer. This function can be
     * called only for objects that were constructed by the constructor that
     * takes a shared pointer argument.
     */
    const result_type &result() const noexcept {
        return m_value.value<result_type>();
    }

}; // class user_provided_trigger

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_user_provided_trigger_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
