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

#ifndef INCLUDED_os_event_pselect_api_hh
#define INCLUDED_os_event_pselect_api_hh

#include "buildconfig.h"

#include <chrono>
#include <memory>
#include <system_error>
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/signal_number_set.hh"
#include "os/time_api.hh"

namespace sesh {
namespace os {
namespace event {

/** Abstraction of the pselect POSIX API function. */
class pselect_api : public virtual time_api {

public:

    /** Returns a unique pointer to a new empty file descriptor set. */
    virtual auto create_file_descriptor_set() const
            -> std::unique_ptr<io::file_descriptor_set> = 0;

    /** Returns a unique pointer to a new empty signal number set. */
    virtual auto create_signal_number_set() const
            -> std::unique_ptr<signaling::signal_number_set> = 0;

    /**
     * Wait for a file descriptor or signal event.
     *
     * The pointer arguments to file descriptor sets and a signal number set
     * may be null. Non-null pointers passed to this function must be obtained
     * from the {@link #create_file_descriptor_set} and {@link
     * #create_signal_number_set} functions called for the same
     * <code>*this</code>.
     *
     * @param timeout A negative value means no timeout.
     */
    virtual std::error_code pselect(
            io::file_descriptor::value_type fd_bound,
            io::file_descriptor_set *read_fds,
            io::file_descriptor_set *write_fds,
            io::file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const signaling::signal_number_set *signal_mask) const = 0;

}; // class pselect_api

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_pselect_api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
