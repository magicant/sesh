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

#ifndef INCLUDED_os_event_Proactor_hh
#define INCLUDED_os_event_Proactor_hh

#include "buildconfig.h"

#include <utility>
#include <vector>
#include "async/Future.hh"
#include "common/ContainerHelper.hh"
#include "os/event/Trigger.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * A proactor accepts requests for future notification that should happen when
 * a specific trigger condition is met.
 */
class Proactor {

protected:

    virtual async::Future<Trigger> expectImpl(std::vector<Trigger> &&) = 0;

public:

    virtual ~Proactor() = default;

    /**
     * Registers a set of event triggers and returns a future that, when one of
     * the trigger condition is met, returns the trigger.
     *
     * Note that the future's callback will never be called if the trigger set
     * is empty.
     *
     * This function returns a future that has not yet received a result unless
     * the argument includes an already-filled user-provided trigger.
     *
     * All the file descriptors contained in the trigger set must be kept valid
     * (open) until the future returned from this function receives a result.
     * Otherwise, the behavior is undefined. That means you must ensure
     * validity of the file descriptors before calling this function.
     *
     * Any {@link UserProvidedTrigger} contained in the trigger set must have
     * been constructed by the constructor that takes a future argument. If a
     * user-provided trigger fires the event, the future that was returned from
     * this function provides a user-provided trigger object that was
     * constructed by the constructor that takes a shared pointer argument.
     *
     * If a file descriptor trigger specifies a too large file descriptor that
     * is not supported by the underlying OS API, the returned future fails
     * with std::domain_error.
     */
    async::Future<Trigger> expect(std::vector<Trigger> &&triggers) {
        return expectImpl(std::move(triggers));
    }

    void expect(std::vector<Trigger> &) = delete;
    void expect(const std::vector<Trigger> &) = delete;

    /**
     * A convenient version of {@link #expect(std::vector<Trigger> &&)} to
     * which you can directly pass triggers as arguments.
     */
    template<typename... TriggerArg>
    async::Future<Trigger> expect(TriggerArg &&... t) {
        return expectImpl(common::createVectorOf<Trigger>(
                std::forward<TriggerArg>(t)...));
    }

}; // class Proactor

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_Proactor_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
