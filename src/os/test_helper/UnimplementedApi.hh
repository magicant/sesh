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

#ifndef INCLUDED_os_test_helper_UnimplementedApi_hh
#define INCLUDED_os_test_helper_UnimplementedApi_hh

#include "buildconfig.h"

#include "os/Api.hh"

namespace sesh {
namespace os {
namespace test_helper {

class UnimplementedApi : public Api {

    SystemClockTime systemClockNow() const noexcept {
        throw "unimplemented systemClockNow";
    }

    SteadyClockTime steadyClockNow() const noexcept {
        throw "unimplemented steadyClockNow";
    }

    std::error_code close(io::FileDescriptor &) const override {
        throw "unimplemented close";
    }

    std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet() const
            override {
        throw "unimplemented createFileDescriptorSet";
    }

    std::unique_ptr<signaling::SignalNumberSet> createSignalNumberSet() const
            override {
        throw "unimplemented createSignalNumberSet";
    }

    std::error_code pselect(
            io::FileDescriptor::Value,
            io::FileDescriptorSet *,
            io::FileDescriptorSet *,
            io::FileDescriptorSet *,
            std::chrono::nanoseconds,
            const signaling::SignalNumberSet *) const override {
        throw "unimplemented createSignalNumberSet";
    }

    std::error_code sigprocmask(
            MaskChangeHow,
            const signaling::SignalNumberSet *,
            signaling::SignalNumberSet *) const override {
        throw "unimplemented sigprocmask";
    }

    std::error_code sigaction(
            signaling::SignalNumber,
            const SignalAction *,
            SignalAction *) const override {
        throw "unimplemented sigaction";
    }

}; // class UnimplementedApi

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_UnimplementedApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
