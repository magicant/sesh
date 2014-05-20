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

#ifndef INCLUDED_os_event_Awaiter_hh
#define INCLUDED_os_event_Awaiter_hh

#include "buildconfig.h"

#include <memory>
#include "os/Api.hh"
#include "os/event/Proactor.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * An awaiter is a proactor that can wait for events to happen and dispatch
 * them to appropriate listeners.
 *
 * An awaiter depends on the p-select and now time API.
 *
 * @see Api
 */
class Awaiter : public Proactor {

public:

    virtual ~Awaiter() = default;

    /**
     * Waits for event trigger conditions to be met and dispatches the trigger
     * information to the corresponding future object that was returned from
     * the expect function. This function will not return until all pending
     * events are processed. This function returns immediately if no events are
     * pending.
     */
    virtual void awaitEvents() = 0;

}; // class Awaiter

std::unique_ptr<Awaiter> createAwaiter(const Api &);

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_Awaiter_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
