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

#ifndef INCLUDED_os_event_UserProvidedTrigger_hh
#define INCLUDED_os_event_UserProvidedTrigger_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "async/Future.hh"
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
class UserProvidedTrigger {

public:

    using Result = std::shared_ptr<void>;

private:

    using Value = common::variant<async::Future<Result>, Result>;

    Value mValue;

public:

    /** Constructs a new user-provided trigger from the given future. */
    explicit UserProvidedTrigger(async::Future<Result> &&f) noexcept :
            mValue(std::move(f)) { }

    /**
     * Constructs a new user-provided trigger that has the given result.
     *
     * User-provided triggers constructed by this constructor must not be used
     * to specify a trigger set.
     */
    explicit UserProvidedTrigger(Result &&r) noexcept :
            mValue(std::move(r)) { }

    /**
     * Returns a reference to the future that triggers the event. This function
     * can be called only for objects that were constructed by the constructor
     * that takes a future argument.
     */
    async::Future<Result> &future() noexcept {
        return mValue.value<async::Future<Result>>();
    }

    /**
     * Returns a reference to the result shared pointer. This function can be
     * called only for objects that were constructed by the constructor that
     * takes a shared pointer argument.
     */
    Result &result() noexcept { return mValue.value<Result>(); }
    /**
     * Returns a reference to the result shared pointer. This function can be
     * called only for objects that were constructed by the constructor that
     * takes a shared pointer argument.
     */
    const Result &result() const noexcept { return mValue.value<Result>(); }

}; // class UserProvidedTrigger

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_UserProvidedTrigger_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
