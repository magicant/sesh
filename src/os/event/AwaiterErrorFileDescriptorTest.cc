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
#include <iterator>
#include <set>
#include <system_error>
#include <utility>
#include "async/Future.hh"
#include "common/Try.hh"
#include "common/Variant.hh"
#include "os/Api.hh"
#include "os/event/Awaiter.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/ErrorFileDescriptor.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/test_helper/PselectApiStub.hh"

namespace {

using sesh::async::Future;
using sesh::common::Try;
using sesh::common::Variant;
using sesh::os::event::Awaiter;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::ErrorFileDescriptor;
using sesh::os::event::PselectAndNowApiStub;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumberSet;
using sesh::os::test_helper::PselectApiStub;

using TimePoint = sesh::os::Api::SteadyClockTime;

std::vector<Trigger> fdTriggers(std::vector<ErrorFileDescriptor> &&fds) {
    return std::vector<Trigger>(
            std::make_move_iterator(fds.begin()),
            std::make_move_iterator(fds.end()));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: awaiting single error FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(ErrorFileDescriptor(4));
    std::move(f).setCallback([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 4);
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {4}, fdBound, "errorFds");
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
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: one trigger set containing different error FDs: "
        "pselect returning single FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(fdTriggers(
            {ErrorFileDescriptor(2), ErrorFileDescriptor(0)}));
    std::move(f).setCallback([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 0);
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {0, 2}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutableSteadyClockNow() += std::chrono::seconds(3);
        errorFds->reset(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutableSteadyClockNow() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: one trigger set containing different error FDs: "
        "pselect returning all FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;
    Future<Trigger> f = a.expect(fdTriggers(
            {ErrorFileDescriptor(2), ErrorFileDescriptor(0)}));
    std::move(f).setCallback([this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        auto fd = t->value<ErrorFileDescriptor>().value();
        if (fd != 0)
            CHECK(fd == 2);
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {0, 2}, fdBound, "errorFds");
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
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: two trigger sets containing different error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutableSteadyClockNow() = startTime;

    a.expect(ErrorFileDescriptor(1)).setCallback(
            [this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 1);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(9));
        mutableSteadyClockNow() += std::chrono::seconds(1);
    });
    a.expect(ErrorFileDescriptor(3)).setCallback(
            [this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 3);
        CHECK(steadyClockNow() == startTime + std::chrono::seconds(28));
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
        checkEmpty(readFds, fdBound, "readFds 1");
        checkEmpty(writeFds, fdBound, "writeFds 1");
        checkEqual(errorFds, {1, 3}, fdBound, "errorFds 1");
        errorFds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);

        mutableSteadyClockNow() += std::chrono::seconds(9);
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
            checkEqual(errorFds, {3}, fdBound, "errorFds 2");
            CHECK(timeout.count() < 0);
            CHECK(signalMask == nullptr);
            mutableSteadyClockNow() += std::chrono::seconds(18);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(30));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: two trigger sets containing same error FD") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutableSteadyClockNow() = startTime;

    auto callback = [this, startTime](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        CHECK(t->index() == Trigger::index<ErrorFileDescriptor>());
        CHECK(t->value<ErrorFileDescriptor>().value() == 7);
        mutableSteadyClockNow() += std::chrono::seconds(1);
    };
    a.expect(ErrorFileDescriptor(7)).setCallback(callback);
    a.expect(ErrorFileDescriptor(7)).setCallback(callback);

    unsigned count = 0;
    implementation() = [this, &count](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        INFO(count);
        ++count;
        checkEmpty(readFds, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {7}, fdBound, "errorFds");
        errorFds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
