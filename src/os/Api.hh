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

#ifndef INCLUDED_os_Api_hh
#define INCLUDED_os_Api_hh

#include "buildconfig.h"

#include <chrono>
#include <memory>
#include <system_error>
#include "common/Variant.hh"
#include "os/capitypes.h"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace sesh {
namespace os {

/** Abstraction of POSIX API. */
class Api {

public:

    using SystemClockTime = std::chrono::time_point<
            std::chrono::system_clock, std::chrono::nanoseconds>;
    using SteadyClockTime = std::chrono::time_point<
            std::chrono::steady_clock, std::chrono::nanoseconds>;

    /** Returns the current time. */
    virtual SystemClockTime systemClockNow() const noexcept = 0;

    /** Returns the current time. */
    virtual SteadyClockTime steadyClockNow() const noexcept = 0;

    /**
     * Closes the given file descriptor. This function may block on some
     * conditions; refer to the POSIX standard for details.
     *
     * On success, the argument file descriptor is invalidated and a
     * default-constructed error condition is returned.
     *
     * On failure, the file descriptor may be left still valid.
     */
    virtual std::error_code close(io::FileDescriptor &) const = 0;

    /** Returns a unique pointer to a new empty file descriptor set. */
    virtual std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet()
            const = 0;

    /** Returns a unique pointer to a new empty signal number set. */
    virtual std::unique_ptr<signaling::SignalNumberSet> createSignalNumberSet()
            const = 0;

    /**
     * Wait for a file descriptor or signal event.
     *
     * The pointer arguments to file descriptor sets and a signal number set
     * may be null. Non-null pointers passed to this function must be obtained
     * from the {@link #createFileDescriptorSet} and {@link
     * #createSignalNumberSet} functions called for the same {@code *this}.
     *
     * @param timeout A negative value means no timeout.
     */
    virtual std::error_code pselect(
            io::FileDescriptor::Value fdBound,
            io::FileDescriptorSet *readFds,
            io::FileDescriptorSet *writeFds,
            io::FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const signaling::SignalNumberSet *signalMask) const = 0;

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
            const signaling::SignalNumberSet *newMask,
            signaling::SignalNumberSet *oldMask) const = 0;

    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmaskBlock(signaling::SignalNumber) const;
    /** Convenience wrapper for {@link #sigprocmask}. */
    std::error_code sigprocmaskUnblock(signaling::SignalNumber) const;

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
            signaling::SignalNumber,
            const SignalAction *newAction,
            SignalAction *oldAction) const = 0;

    /** Reference to the only instance of real API implementation. */
    static const Api &INSTANCE;

}; // class Api

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_Api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
