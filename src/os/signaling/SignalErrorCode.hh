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

#ifndef INCLUDED_os_signaling_SignalErrorCode_hh
#define INCLUDED_os_signaling_SignalErrorCode_hh

#include "buildconfig.h"

#include <system_error>
#include <type_traits>
#include "os/signaling/signal_error_category.hh"

namespace sesh {
namespace os {
namespace signaling {

/** Error codes for the signal error category. */
enum class SignalErrorCode {

    /**
     * Reports that the trap action could not be changed because the initial
     * action was set to "ignore."
     */
    INITIALLY_IGNORED = 1,

}; // enum class SignalErrorCode

inline std::error_code make_error_code(SignalErrorCode c) noexcept {
    return std::error_code(static_cast<int>(c), signal_error_category);
}

} // namespace signaling
} // namespace os
} // namespace sesh

namespace std {

template<>
struct is_error_code_enum<sesh::os::signaling::SignalErrorCode> :
        public true_type {
};

} // namespace std

#endif // #ifndef INCLUDED_os_signaling_SignalErrorCode_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
