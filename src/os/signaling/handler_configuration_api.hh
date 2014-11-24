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

#ifndef INCLUDED_os_signaling_handler_configuration_api_hh
#define INCLUDED_os_signaling_handler_configuration_api_hh

#include "buildconfig.h"

#include <memory>
#include <system_error>
#include "common/variant.hh"
#include "os/capitypes.h"
#include "os/signaling/signal_number.hh"
#include "os/signaling/signal_number_set.hh"

namespace sesh {
namespace os {
namespace signaling {

/** Abstraction of POSIX API that manipulates signal mask and handlers. */
class handler_configuration_api {

public:

    /** Returns a unique pointer to a new empty signal number set. */
    virtual std::unique_ptr<signal_number_set> create_signal_number_set() const
            = 0;

    enum class mask_change_how { block, unblock, set_mask };

    /**
     * Changes the signal blocking mask.
     *
     * The pointer arguments to signal number sets may be null. Non-null
     * pointers passed to this function must be obtained from the {@link
     * #create_signal_number_set} functions called for the same
     * <code>*this</code>.
     */
    virtual std::error_code sigprocmask(
            mask_change_how,
            const signal_number_set *new_mask,
            signal_number_set *old_mask) const = 0;

    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmask_block(signal_number) const;
    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmask_unblock(signal_number) const;

    class default_action { };
    class ignore { };

    using signal_action = common::variant<
            default_action, ignore, sesh_osapi_signal_handler *>;

    /**
     * Changes and/or queries the signal handler setting for a signal.
     *
     * This function currently allows setting the signal handler function only.
     * The @c sa_mask set and @c sa_flags are considered empty.
     */
    virtual std::error_code sigaction(
            signal_number,
            const signal_action *new_action,
            signal_action *old_action) const = 0;

}; // class handler_configuration_api

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_handler_configuration_api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
