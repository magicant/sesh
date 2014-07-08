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

#ifndef INCLUDED_os_event_PselectApi_hh
#define INCLUDED_os_event_PselectApi_hh

#include "buildconfig.h"

#include <chrono>
#include <memory>
#include <system_error>
#include "os/TimeApi.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace sesh {
namespace os {
namespace event {

/** Abstraction of the pselect POSIX API function. */
class PselectApi : public virtual TimeApi {

public:

    /** Returns a unique pointer to a new empty file descriptor set. */
    virtual std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet()
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

}; // class PselectApi

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_PselectApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
