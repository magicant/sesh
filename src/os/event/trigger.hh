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

#ifndef INCLUDED_os_event_trigger_hh
#define INCLUDED_os_event_trigger_hh

#include "buildconfig.h"

#include "common/variant.hh"
#include "os/event/error_file_descriptor.hh"
#include "os/event/readable_file_descriptor.hh"
#include "os/event/signal.hh"
#include "os/event/timeout.hh"
#include "os/event/UserProvidedTrigger.hh"
#include "os/event/WritableFileDescriptor.hh"

namespace sesh {
namespace os {
namespace event {

/** A trigger describes the cause of an event. */
using trigger = common::variant<
        timeout,
        readable_file_descriptor,
        WritableFileDescriptor,
        error_file_descriptor,
        signal,
        UserProvidedTrigger>;

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_trigger_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
