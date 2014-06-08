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

#ifndef INCLUDED_os_test_helper_PselectApiStub_hh
#define INCLUDED_os_test_helper_PselectApiStub_hh

#include "buildconfig.h"

#include <functional>
#include "os/test_helper/FileDescriptorSetApi.hh"

namespace sesh {
namespace os {
namespace test_helper {

class PselectApiStub : public virtual FileDescriptorSetApi {

public:

    using PselectFunction = std::function<std::error_code(
            const PselectApiStub &,
            io::FileDescriptor::Value,
            io::FileDescriptorSet *,
            io::FileDescriptorSet *,
            io::FileDescriptorSet *,
            std::chrono::nanoseconds,
            const signaling::SignalNumberSet *)>;

private:

    mutable PselectFunction mImplementation;

public:

    PselectFunction &implementation() const { return mImplementation; }

    std::error_code pselect(
            io::FileDescriptor::Value fdBound,
            io::FileDescriptorSet *readFds,
            io::FileDescriptorSet *writeFds,
            io::FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const signaling::SignalNumberSet *signalMask) const override {
        return mImplementation(
                *this,
                fdBound,
                readFds,
                writeFds,
                errorFds,
                timeout,
                signalMask);
    }

}; // class PselectApiStub

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_PselectApiStub_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */