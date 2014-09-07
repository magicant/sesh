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

#ifndef INCLUDED_os_time_api_test_helper_hh
#define INCLUDED_os_time_api_test_helper_hh

#include "buildconfig.h"

#include "os/time_api.hh"

namespace sesh {
namespace os {

class time_api_fake : public virtual time_api {

private:

    system_clock_time m_system_clock_now;
    steady_clock_time m_steady_clock_now;

public:

    system_clock_time &mutable_system_clock_now() noexcept {
        return m_system_clock_now;
    }

    steady_clock_time &mutable_steady_clock_now() noexcept {
        return m_steady_clock_now;
    }

    system_clock_time system_clock_now() const noexcept override {
        return m_system_clock_now;
    }

    steady_clock_time steady_clock_now() const noexcept override {
        return m_steady_clock_now;
    }

}; // class time_api_fake

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_time_api_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
