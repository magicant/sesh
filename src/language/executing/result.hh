/* Copyright (C) 2015 WATANABE Yuki
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

#ifndef INCLUDED_language_executing_result_hh
#define INCLUDED_language_executing_result_hh

#include "buildconfig.h"

#include "common/either.hh"
#include "common/variant.hh"
#include "os/signaling/signal_number.hh"

namespace sesh {
namespace language {
namespace executing {

/** Wrapper for exit status numbers. */
class exit_status {

public:

    using value_type = int;

    value_type value;

    constexpr exit_status(value_type v) noexcept : value(v) { }

}; // class exit_status

/** Wrapper for signal numbers. */
class signal_number {

public:

    using value_type = os::signaling::signal_number;

    value_type value;

    constexpr signal_number(value_type v) noexcept : value(v) { }

}; // class signal_number

/**
 * This variant denotes how a process was terminated. A process can be
 * terminated either by exiting or by receiving a signal. 
 *
 * Some kinds of signals suspend or resume a process instead of terminating it.
 * This variant may contain such a non-terminating signal depending on the
 * context in which the variant is used.
 */
using process_termination = common::variant<exit_status, signal_number>;

/** Result of execution. */
class result {

public:

    /** Result of the last terminated process, if any process was executed. */
    common::maybe<process_termination> last_process_termination;

    result() noexcept : last_process_termination() { }

}; // class result

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_result_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
