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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>
#include <exception>
#include "async/Future.hh"
#include "common/Try.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Signal.hh"
#include "os/event/Timeout.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/test_helper/SigactionApiFake.hh"
#include "os/test_helper/SignalMaskApiFake.hh"

/*
namespace std {

template<typename Char, typename Traits, typename R, typename P>
std::ostream &operator<<(
        std::basic_ostream<Char, Traits> &s, std::chrono::duration<R, P> d) {
    s << std::chrono::duration_cast<std::chrono::nanoseconds>(d).count() <<
            " ns";
    return s;
}

template<typename Char, typename Traits, typename C, typename D>
std::ostream &operator<<(
        std::basic_ostream<Char, Traits> &s, std::chrono::time_point<C, D> t) {
    return s << t.time_since_epoch() << " since epoch";
}

}
*/

namespace {

using sesh::common::Try;
using sesh::async::Future;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::PselectAndNowApiStub;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Signal;
using sesh::os::event::Timeout;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumberSet;
using sesh::os::test_helper::SigactionApiFake;
using sesh::os::test_helper::SignalMaskApiFake;

using TimePoint = sesh::os::Api::SteadyClockTime;

class SignalAwaiterTestApiFake :
        public PselectAndNowApiStub,
        public virtual SigactionApiFake,
        public virtual SignalMaskApiFake {
};

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: timeout with FD trigger in same set") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(
            Timeout(std::chrono::seconds(5)), ReadableFileDescriptor(3));
    std::move(f).then([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->index() == t->index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(5));
        mutableSteadyClockNow() += std::chrono::seconds(2);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {3}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::seconds(4));
        CHECK(signalMask == nullptr);
        readFds->reset();
        mutableSteadyClockNow() += std::chrono::seconds(4);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(1);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(7));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: FD trigger with timeout in same set") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(
            Timeout(std::chrono::seconds(10)), ReadableFileDescriptor(3));
    std::move(f).then([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->index() == t->index<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 3);
        mutableSteadyClockNow() += std::chrono::seconds(4);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {3}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::seconds(9));
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(1);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(7));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: signal handler is reset after event fired (with timeout)") {
    a.expect(Timeout(std::chrono::seconds(1)), Signal(1));

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        Action &a = actions().at(1);
        CHECK(a.index() == Action::index<sesh_osapi_signal_handler *>());

        mutableSteadyClockNow() += std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    a.awaitEvents();

    Action &a = actions().at(1);
    CHECK(a.index() == Action::index<Default>());
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: setting timeout from domain error") {
    auto fd = ReadableFileDescriptor(FileDescriptorSetImpl::MAX_VALUE + 1);
    bool called = false;
    a.expect(fd).wrap().recover([this](std::exception_ptr) {
        return a.expect(Timeout(std::chrono::seconds(0)));
    }).unwrap().then([&](Try<Trigger> &&) {
        called = true;
    });
    a.awaitEvents();
    CHECK(called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
