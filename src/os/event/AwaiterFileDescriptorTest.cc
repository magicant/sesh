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

#include <stdexcept>
#include "async/Future.hh"
#include "common/Try.hh"
#include "common/TypeTagTestHelper.hh"
#include "common/Variant.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/ErrorFileDescriptor.hh"
#include "os/event/PselectApi.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Signal.hh"
#include "os/event/Timeout.hh"
#include "os/event/Trigger.hh"
#include "os/event/UserProvidedTrigger.hh"
#include "os/event/WritableFileDescriptor.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::Future;
using sesh::common::Try;
using sesh::common::Variant;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::ErrorFileDescriptor;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Signal;
using sesh::os::event::Timeout;
using sesh::os::event::Trigger;
using sesh::os::event::UserProvidedTrigger;
using sesh::os::event::WritableFileDescriptor;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiDummy;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::SteadyClockTime;
using TriggerFileDescriptor = Variant<
        ReadableFileDescriptor, WritableFileDescriptor, ErrorFileDescriptor>;

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing readable and writable FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(
            ReadableFileDescriptor(3), WritableFileDescriptor(3));
    std::move(f).then([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        switch (t->tag()) {
        case Trigger::tag<ReadableFileDescriptor>():
            CHECK(t->value<ReadableFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<WritableFileDescriptor>():
            CHECK(t->value<WritableFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<ErrorFileDescriptor>():
        case Trigger::tag<Timeout>():
        case Trigger::tag<Signal>():
        case Trigger::tag<UserProvidedTrigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(5));
        mutableSteadyClockNow() += std::chrono::seconds(1);
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
        checkEqual(writeFds, {3}, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing readable and error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(
            ReadableFileDescriptor(3), ErrorFileDescriptor(3));
    std::move(f).then([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        switch (t->tag()) {
        case Trigger::tag<ReadableFileDescriptor>():
            CHECK(t->value<ReadableFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<ErrorFileDescriptor>():
            CHECK(t->value<ErrorFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<WritableFileDescriptor>():
        case Trigger::tag<Timeout>():
        case Trigger::tag<Signal>():
        case Trigger::tag<UserProvidedTrigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(5));
        mutableSteadyClockNow() += std::chrono::seconds(1);
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
        checkEqual(errorFds, {3}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing writable and error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(
            WritableFileDescriptor(3), ErrorFileDescriptor(3));
    std::move(f).then([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        switch (t->tag()) {
        case Trigger::tag<WritableFileDescriptor>():
            CHECK(t->value<WritableFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<ErrorFileDescriptor>():
            CHECK(t->value<ErrorFileDescriptor>().value() == 3);
            break;
        case Trigger::tag<ReadableFileDescriptor>():
        case Trigger::tag<Timeout>():
        case Trigger::tag<Signal>():
        case Trigger::tag<UserProvidedTrigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(5));
        mutableSteadyClockNow() += std::chrono::seconds(1);
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
        checkEqual(writeFds, {3}, fdBound, "writeFds");
        checkEqual(errorFds, {3}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing readable and writable FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutableSteadyClockNow() = startTime;

    bool callback1Called = false, callback2Called = false;
    a.expect(ReadableFileDescriptor(2)).then(
            [this, startTime, &callback1Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 2);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(WritableFileDescriptor(3)).then(
            [this, startTime, &callback2Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 3);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, startTime](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutableSteadyClockNow() = startTime + std::chrono::seconds(10);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing writable and error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutableSteadyClockNow() = startTime;

    bool callback1Called = false, callback2Called = false;
    a.expect(WritableFileDescriptor(3)).then(
            [this, startTime, &callback1Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 3);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(ErrorFileDescriptor(2)).then(
            [this, startTime, &callback2Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 2);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, startTime](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutableSteadyClockNow() = startTime + std::chrono::seconds(10);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing readable and error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutableSteadyClockNow() = startTime;

    bool callback1Called = false, callback2Called = false;
    a.expect(ReadableFileDescriptor(2)).then(
            [this, startTime, &callback1Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 2);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(ErrorFileDescriptor(3)).then(
            [this, startTime, &callback2Called](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 3);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, startTime](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutableSteadyClockNow() = startTime + std::chrono::seconds(10);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: awaiting max readable FD") {
    auto max = FileDescriptorSetImpl::MAX_VALUE;
    bool callbackCalled = false;
    a.expect(ReadableFileDescriptor(max)).then(
            [this, max, &callbackCalled](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == max);
        callbackCalled = true;
    });

    implementation() = [](
            const PselectApiStub &,
            FileDescriptor::Value,
            FileDescriptorSet *,
            FileDescriptorSet *,
            FileDescriptorSet *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: domain error from FD set") {
    auto max = FileDescriptorSetImpl::MAX_VALUE;
    bool callbackCalled = false;
    a.expect(ReadableFileDescriptor(max + 1)).then(
            [this, &callbackCalled](Try<Trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &) {
            callbackCalled = true;
        }
    });

    a.awaitEvents();
    CHECK(callbackCalled);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
