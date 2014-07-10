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

#ifndef INCLUDED_os_signaling_HandlerConfigurationApi_hh
#define INCLUDED_os_signaling_HandlerConfigurationApi_hh

#include "buildconfig.h"

#include <memory>
#include <system_error>
#include "common/Variant.hh"
#include "os/capitypes.h"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace sesh {
namespace os {
namespace signaling {

/** Abstraction of POSIX API that manipulates signal mask and handlers. */
class HandlerConfigurationApi {

public:

    /** Returns a unique pointer to a new empty signal number set. */
    virtual std::unique_ptr<SignalNumberSet> createSignalNumberSet() const = 0;

    enum class MaskChangeHow { BLOCK, UNBLOCK, SET_MASK };

    /**
     * Changes the signal blocking mask.
     *
     * The pointer arguments to signal number sets may be null. Non-null
     * pointers passed to this function must be obtained from the {@link
     * #createSignalNumberSet} functions called for the same {@code *this}.
     */
    virtual std::error_code sigprocmask(
            MaskChangeHow,
            const SignalNumberSet *newMask,
            SignalNumberSet *oldMask) const = 0;

    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmaskBlock(SignalNumber) const;
    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmaskUnblock(SignalNumber) const;

    class Default { };
    class Ignore { };

    using SignalAction =
            common::Variant<Default, Ignore, sesh_osapi_signal_handler *>;

    /**
     * Changes and/or queries the signal handler setting for a signal.
     *
     * This function currently allows setting the signal handler function only.
     * The {@code sa_mask} set and {@code sa_flags} are considered empty.
     */
    virtual std::error_code sigaction(
            SignalNumber,
            const SignalAction *newAction,
            SignalAction *oldAction) const = 0;

}; // class HandlerConfigurationApi

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_HandlerConfigurationApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
