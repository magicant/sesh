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

#ifndef INCLUDED_os_event_PselectApiTestHelper_hh
#define INCLUDED_os_event_PselectApiTestHelper_hh

#include "buildconfig.h"

#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include "os/event/PselectApi.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/io/FileDescriptorSetTestHelper.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/time_api_test_helper.hh"

/*
#include <ostream>

namespace std {

template<typename ChatT, typename Traits>
std::basic_ostream<ChatT, Traits> &operator<<(
        std::basic_ostream<ChatT, Traits> &os,
        const std::set<sesh::os::io::FileDescriptor::Value> &fds) {
    os << '{';
    for (const auto &fd : fds)
        os << fd << ',';
    return os << '}';
}

} // namespace std
*/

namespace sesh {
namespace os {
namespace event {

class PselectApiStub : public PselectApi, public time_api_fake {

public:

    using FileDescriptorSetImpl = io::FileDescriptorSetFake;

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

    std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet() const
            override {
        return std::unique_ptr<io::FileDescriptorSet>(
                new FileDescriptorSetImpl());
    }

    static void checkEqual(
            const io::FileDescriptorSet *actual,
            const std::set<io::FileDescriptor::Value> &expected,
            io::FileDescriptor::Value actualBound,
            const std::string &info) {
        INFO(info);

        const FileDescriptorSetImpl emptySet{};
        if (actual == nullptr)
            actual = &emptySet;

        const FileDescriptorSetImpl *actualImpl =
                dynamic_cast<const FileDescriptorSetImpl *>(actual);
        REQUIRE(actualImpl != nullptr);
        CHECK(*actualImpl == expected);
        CHECK(actualImpl->bound() <= actualBound);
    }

    static void checkEmpty(
            const io::FileDescriptorSet *fds,
            io::FileDescriptor::Value fdBound,
            const std::string &info) {
        checkEqual(fds, std::set<io::FileDescriptor::Value>{}, fdBound, info);
    }

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

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_PselectApiTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
