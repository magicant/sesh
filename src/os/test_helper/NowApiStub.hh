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

#ifndef INCLUDED_os_test_helper_NowApiStub_hh
#define INCLUDED_os_test_helper_NowApiStub_hh

#include "buildconfig.h"

#include "os/test_helper/UnimplementedApi.hh"

namespace sesh {
namespace os {
namespace test_helper {

class NowApiStub : public virtual UnimplementedApi {

private:

    SystemClockTime mSystemClockNow;
    SteadyClockTime mSteadyClockNow;

public:

    void setSystemClockNow(SystemClockTime now) noexcept {
        mSystemClockNow = now;
    }

    void setSteadyClockNow(SteadyClockTime now) noexcept {
        mSteadyClockNow = now;
    }

    SystemClockTime &mutableSystemClockNow() noexcept {
        return mSystemClockNow;
    }

    SteadyClockTime &mutableSteadyClockNow() noexcept {
        return mSteadyClockNow;
    }

    SystemClockTime systemClockNow() const noexcept override {
        return mSystemClockNow;
    }

    SteadyClockTime steadyClockNow() const noexcept override {
        return mSteadyClockNow;
    }

}; // class NowApiStub

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_NowApiStub_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
