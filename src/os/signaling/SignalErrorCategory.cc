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

#include "buildconfig.h"
#include "SignalErrorCategory.hh"

#include <string>
#include <system_error>

namespace sesh {
namespace os {
namespace signaling {

class SignalErrorCategory : public std::error_category {

public:

    const char *name() const noexcept final override {
        return "signal";
    }

    /** Always returns "?". */
    std::string message(int) const final override {
        return "?";
    }

}; // class SignalErrorCategory

const std::error_category &SIGNAL_ERROR_CATEGORY =
        SignalErrorCategory();

} // namespace signaling
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
