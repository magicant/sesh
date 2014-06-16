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
#include <utility>
#include <vector>
#include "async/Future.hh"
#include "common/Try.hh"
#include "os/Api.hh"
#include "os/event/Awaiter.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/Signal.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/test_helper/SigactionApiFake.hh"
#include "os/test_helper/SignalMaskApiFake.hh"

namespace {

using sesh::async::Future;
using sesh::common::Try;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::PselectAndNowApiStub;
using sesh::os::event::Signal;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;
using sesh::os::test_helper::SigactionApiFake;
using sesh::os::test_helper::SignalMaskApiFake;

using TimePoint = sesh::os::Api::SteadyClockTime;

class SignalAwaiterTestApiFake :
        public PselectAndNowApiStub,
        public virtual SigactionApiFake,
        public virtual SignalMaskApiFake {
};

std::vector<Trigger> signalTriggers(std::vector<Signal> &&signals) {
    return std::vector<Trigger>(
            std::make_move_iterator(signals.begin()),
            std::make_move_iterator(signals.end()));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: one signal in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(Signal(3));
    std::move(f).setCallback([this](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->index() == Trigger::index<Signal>());
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
        REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
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
        REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        implementation() = nullptr;
        return std::error_code();
    };
    a.awaitEvents();
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: two signals in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(100));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(signalTriggers({Signal(2), Signal(6)}));
    std::move(f).setCallback([this](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->index() == Trigger::index<Signal>());
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
        REQUIRE(a2.index() == Action::index<sesh_osapi_signal_handler *>());

        Action &a6 = actions().at(6);
        REQUIRE(a6.index() == Action::index<sesh_osapi_signal_handler *>());
        a6.value<sesh_osapi_signal_handler *>()(6);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: same signal in two trigger sets") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (unsigned i = 0; i < 2; ++i) {
        a.expect(Signal(1)).setCallback([this](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->index() == Trigger::index<Signal>());
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
        REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: different signals in two trigger sets: fired at a time") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (SignalNumber sn : {1, 2}) {
        a.expect(Signal(sn)).setCallback([this, sn](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->index() == Trigger::index<Signal>());
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
        REQUIRE(a1.index() == Action::index<sesh_osapi_signal_handler *>());
        a1.value<sesh_osapi_signal_handler *>()(1);

        Action &a2 = actions().at(2);
        REQUIRE(a2.index() == Action::index<sesh_osapi_signal_handler *>());
        a2.value<sesh_osapi_signal_handler *>()(2);

        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
        "Awaiter: different signals in two trigger sets: "
        "fired intermittently") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    for (SignalNumber sn : {1, 2}) {
        a.expect(Signal(sn)).setCallback([this, sn](Try<Trigger> &&t) {
            REQUIRE(t.hasValue());
            REQUIRE(t->index() == Trigger::index<Signal>());
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
        REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
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
            REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
            a.value<sesh_osapi_signal_handler *>()(2);

            mutableSteadyClockNow() += std::chrono::seconds(3);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(7);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(15));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<SignalAwaiterTestApiFake>,
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
        REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        implementation() = nullptr;
        return std::error_code();
    };
    a.awaitEvents();

    Action &a = actions().at(1);
    CHECK(a.index() == Action::index<Default>());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */