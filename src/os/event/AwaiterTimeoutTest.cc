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
#include <set>
#include <string>
#include <system_error>
#include <utility>
#include "async/Future.hh"
#include "common/ContainerHelper.hh"
#include "common/Try.hh"
#include "os/event/Awaiter.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Timeout.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/test_helper/NowApiStub.hh"
#include "os/test_helper/PselectApiStub.hh"
#include "os/test_helper/UnimplementedApi.hh"

namespace {

using sesh::async::Future;
using sesh::common::Try;
using sesh::common::contains;
using sesh::os::event::Awaiter;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Timeout;
using sesh::os::event::Trigger;
using sesh::os::event::createAwaiter;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumberSet;
using sesh::os::test_helper::NowApiStub;
using sesh::os::test_helper::PselectApiStub;
using sesh::os::test_helper::UnimplementedApi;

using TimePoint = NowApiStub::SteadyClockTime;

template<typename Api>
class Fixture : protected Api {

private:

    std::unique_ptr<Awaiter> mAwaiter = createAwaiter(*this);

protected:

    Awaiter &a = *mAwaiter;

}; // template<typename Api> class Fixture

void checkEqual(
        const FileDescriptorSet *fds,
        const std::set<FileDescriptor::Value> &fdValues,
        FileDescriptor::Value fdBound,
        const std::string &info) {
    INFO(info);

    if (fds == nullptr) {
        CHECK(fdValues.empty());
        return;
    }

    for (FileDescriptor::Value fd = 0; fd < fdBound; ++fd) {
        INFO("fd=" << fd);
        CHECK(fds->test(fd) == contains(fdValues, fd));
    }
}

void checkEmpty(
        const FileDescriptorSet *fds,
        FileDescriptor::Value fdBound,
        const std::string &info) {
    checkEqual(fds, std::set<FileDescriptor::Value>{}, fdBound, info);
}

TEST_CASE_METHOD(
        Fixture<UnimplementedApi>,
        "Awaiter: doesn't wait if no events are pending") {
    a.awaitEvents();
}

TEST_CASE_METHOD(
        Fixture<UnimplementedApi>,
        "Awaiter: does nothing for empty trigger set") {
    Future<Trigger> f = a.expect(std::vector<Trigger>{});
    std::move(f).setCallback([](Try<Trigger> &&) { FAIL("callback called"); });
    a.awaitEvents();
}

class PselectAndNowApiStub : public PselectApiStub, public NowApiStub {
};

template<int durationInSecondsInt>
class TimeoutTest : protected Fixture<PselectAndNowApiStub> {

protected:

    constexpr static std::chrono::seconds duration() noexcept {
        return std::chrono::seconds(durationInSecondsInt);
    }

public:

    TimeoutTest();

}; // class TimeoutTest

template<int durationInSecondsInt>
TimeoutTest<durationInSecondsInt>::TimeoutTest() {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Timeout(duration()));
    bool callbackCalled = false;
    std::move(f).setCallback(
            [this, startTime, &callbackCalled](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == duration());
        CHECK(steadyClockNow() == startTime + duration());
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout == duration() - std::chrono::seconds(1));
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += duration() - std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(1);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + duration());
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(TimeoutTest<1>, "Awaiter: timeout 1") { }

TEST_CASE_METHOD(TimeoutTest<2>, "Awaiter: timeout 2") { }

TEST_CASE_METHOD(Fixture<PselectAndNowApiStub>, "Awaiter: negative timeout") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Timeout(std::chrono::seconds(-10)));
    bool callbackCalled = false;
    std::move(f).setCallback(
            [this, startTime, &callbackCalled](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(-10));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(1));
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::nanoseconds::zero());
        CHECK(signalMask == nullptr);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(1);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(1));
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(
        Fixture<PselectAndNowApiStub>,
        "Awaiter: duplicate timeouts in one trigger set") {
    std::vector<Trigger> triggers;
    triggers.push_back(Timeout(std::chrono::seconds(10)));
    triggers.push_back(Timeout(std::chrono::seconds(5)));
    triggers.push_back(Timeout(std::chrono::seconds(20)));

    auto startTime = TimePoint(std::chrono::seconds(-100));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(std::move(triggers));
    bool callbackCalled = false;
    std::move(f).setCallback(
            [this, startTime, &callbackCalled](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(5));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(5));
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::seconds(5));
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(5);
        implementation() = nullptr;
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(5));
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(
        Fixture<PselectAndNowApiStub>, "Awaiter: two simultaneous timeouts") {
    auto startTime = TimePoint(std::chrono::seconds(1000));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f1 = a.expect(Timeout(std::chrono::seconds(10)));
    bool callback1Called = false;
    std::move(f1).setCallback(
            [this, startTime, &callback1Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(10));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(11));
        callback1Called = true;
    });
    CHECK_FALSE(callback1Called);

    mutableSteadyClockNow() = startTime + std::chrono::seconds(1);
    Future<Trigger> f2 = a.expect(Timeout(std::chrono::seconds(29)));
    bool callback2Called = false;
    std::move(f2).setCallback(
            [this, startTime, &callback2Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(29));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(32));
        callback2Called = true;
    });
    CHECK_FALSE(callback2Called);

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEmpty(readFds, fdBound, "readFds 1");
        checkEmpty(writeFds, fdBound, "writeFds 1");
        checkEmpty(errorFds, fdBound, "errorFds 1");
        CHECK(timeout == std::chrono::seconds(9));
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(10);

        implementation() = [this](
                const PselectApiStub &,
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) -> std::error_code {
            checkEmpty(readFds, fdBound, "readFds 2");
            checkEmpty(writeFds, fdBound, "writeFds 2");
            checkEmpty(errorFds, fdBound, "errorFds 2");
            CHECK(timeout == std::chrono::seconds(19));
            CHECK(signalMask == nullptr);
            mutableSteadyClockNow() += std::chrono::seconds(21);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(32));
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        Fixture<PselectAndNowApiStub>, "Awaiter: two successive timeouts") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Timeout(std::chrono::seconds(100)));
    bool callbackCalled = false;
    std::move(f).map([this, startTime](Trigger &&t) -> Future<Trigger> {
        CHECK(t.index() == Trigger::index<Timeout>());
        CHECK(t.value<Timeout>().interval() == std::chrono::seconds(100));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(102));

        Future<Trigger> f2 = a.expect(Timeout(std::chrono::seconds(8)));
        mutableSteadyClockNow() += std::chrono::seconds(1);
        return f2;
    }).unwrap().setCallback(
            [this, startTime, &callbackCalled](Try<Trigger> &&t) {
        CHECK(t->index() == Trigger::index<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(8));
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(113));

        callbackCalled = true;
        mutableSteadyClockNow() += std::chrono::seconds(2);
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEmpty(readFds, fdBound, "readFds 1");
        checkEmpty(writeFds, fdBound, "writeFds 1");
        checkEmpty(errorFds, fdBound, "errorFds 1");
        CHECK(timeout == std::chrono::seconds(99));
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(101);

        implementation() = [this](
                const PselectApiStub &,
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) -> std::error_code {
            checkEmpty(readFds, fdBound, "readFds 2");
            checkEmpty(writeFds, fdBound, "writeFds 2");
            checkEmpty(errorFds, fdBound, "errorFds 2");
            CHECK(timeout == std::chrono::seconds(7));
            CHECK(signalMask == nullptr);
            mutableSteadyClockNow() += std::chrono::seconds(10);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(1);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(115));
    CHECK(callbackCalled);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
