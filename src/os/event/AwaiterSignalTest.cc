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
#include <system_error>
#include <utility>
#include <vector>
#include "async/Future.hh"
#include "common/Try.hh"
#include "common/TypeTagTestHelper.hh"
#include "os/event/Awaiter.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/PselectApi.hh"
#include "os/event/Signal.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::Future;
using sesh::common::Try;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::Signal;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiFake;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::SteadyClockTime;

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: one signal in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Signal(3));
    std::move(f).then([this](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->tag() == Trigger::tag<Signal>());
        CHECK(t->value<Signal>().number() == 3);
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
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        if (signalMask != nullptr)
            CHECK_FALSE(signalMask->test(3));

        Action &a = actions().at(3);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: irrelevant signals are masked") {
    a.expect(Signal(3));

    signalMask().set(2);
    signalMask().set(5);
    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *maskWhileAwaiting) -> std::error_code {
        if (maskWhileAwaiting != nullptr)
            CHECK_FALSE(maskWhileAwaiting->test(3));
        CHECK(signalMask().test(2));
        CHECK(signalMask().test(5));

        Action &a = actions().at(3);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    a.awaitEvents();
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: two signals in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(100));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Signal(2), Signal(6));
    std::move(f).then([this](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->tag() == Trigger::tag<Signal>());
        CHECK(t->value<Signal>().number() == 6);
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
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        if (signalMask != nullptr) {
            CHECK_FALSE(signalMask->test(3));
            CHECK_FALSE(signalMask->test(6));
        }

        Action &a2 = actions().at(2);
        REQUIRE(a2.tag() == Action::tag<sesh_osapi_signal_handler *>());

        Action &a6 = actions().at(6);
        REQUIRE(a6.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a6.value<sesh_osapi_signal_handler *>()(6);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: same signal in two trigger sets") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (unsigned i = 0; i < 2; ++i) {
        a.expect(Signal(1)).then([this](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->tag() == Trigger::tag<Signal>());
            CHECK(t->value<Signal>().number() == 1);
            mutableSteadyClockNow() += std::chrono::seconds(1);
        });
    }

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
        CHECK(timeout.count() < 0);
        if (signalMask != nullptr)
            CHECK_FALSE(signalMask->test(1));

        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: different signals in two trigger sets: fired at a time") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (SignalNumber sn : {1, 2}) {
        a.expect(Signal(sn)).then([this, sn](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->tag() == Trigger::tag<Signal>());
            CHECK(t->value<Signal>().number() == sn);
            mutableSteadyClockNow() += std::chrono::seconds(1);
        });
    }

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *signalMask) -> std::error_code {
        if (signalMask != nullptr) {
            CHECK_FALSE(signalMask->test(1));
            CHECK_FALSE(signalMask->test(2));
        }

        Action &a1 = actions().at(1);
        REQUIRE(a1.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a1.value<sesh_osapi_signal_handler *>()(1);

        Action &a2 = actions().at(2);
        REQUIRE(a2.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a2.value<sesh_osapi_signal_handler *>()(2);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: different signals in two trigger sets: "
        "fired intermittently") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (SignalNumber sn : {1, 2}) {
        a.expect(Signal(sn)).then([this, sn](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->tag() == Trigger::tag<Signal>());
            CHECK(t->value<Signal>().number() == sn);
            mutableSteadyClockNow() += std::chrono::seconds(1);
        });
    }

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *signalMask) -> std::error_code {
        if (signalMask != nullptr) {
            CHECK_FALSE(signalMask->test(1));
            CHECK_FALSE(signalMask->test(2));
        }

        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = [this](
                const PselectApiStub &,
                FileDescriptor::Value,
                FileDescriptorSet *,
                FileDescriptorSet *,
                FileDescriptorSet *,
                std::chrono::nanoseconds,
                const SignalNumberSet *signalMask) -> std::error_code {
            if (signalMask != nullptr)
                CHECK_FALSE(signalMask->test(2));

            Action &a = actions().at(2);
            REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
            a.value<sesh_osapi_signal_handler *>()(2);

            mutableSteadyClockNow() += std::chrono::seconds(3);
            implementation() = nullptr;
            return std::make_error_code(std::errc::interrupted);
        };
        return std::make_error_code(std::errc::interrupted);
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(15));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiFake>,
        "Awaiter: signal handler is reset after event fired") {
    a.expect(Signal(1));

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    a.awaitEvents();

    Action &a = actions().at(1);
    CHECK(a.tag() == Action::tag<Default>());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
