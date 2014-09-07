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
#include "async/future.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/awaiter.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/PselectApi.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::future;
using sesh::common::trial;
using sesh::os::event::Awaiter;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiDummy;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::steady_clock_time;

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: awaiting single readable FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(ReadableFileDescriptor(4));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 4);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {4}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing different readable FDs: "
        "pselect returning single FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            ReadableFileDescriptor(2), ReadableFileDescriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 0);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {0, 2}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        readFds->reset(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing different readable FDs: "
        "pselect returning all FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            ReadableFileDescriptor(2), ReadableFileDescriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        auto fd = t->value<ReadableFileDescriptor>().value();
        if (fd != 0)
            CHECK(fd == 2);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {0, 2}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing different readable FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    a.expect(ReadableFileDescriptor(1)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 1);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(9));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });
    a.expect(ReadableFileDescriptor(3)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 3);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(28));
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    implementation() = [this](
            const PselectApiStub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        checkEqual(readFds, {1, 3}, fdBound, "readFds 1");
        checkEmpty(writeFds, fdBound, "writeFds 1");
        checkEmpty(errorFds, fdBound, "errorFds 1");
        readFds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);

        mutable_steady_clock_now() += std::chrono::seconds(9);
        implementation() = [this](
                const PselectApiStub &,
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) -> std::error_code {
            checkEqual(readFds, {3}, fdBound, "readFds 2");
            checkEmpty(writeFds, fdBound, "writeFds 2");
            checkEmpty(errorFds, fdBound, "errorFds 2");
            CHECK(timeout.count() < 0);
            CHECK(signalMask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(18);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(30));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing same readable FD") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    auto callback = [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<ReadableFileDescriptor>());
        CHECK(t->value<ReadableFileDescriptor>().value() == 7);
        mutable_steady_clock_now() += std::chrono::seconds(1);
    };
    a.expect(ReadableFileDescriptor(7)).then(callback);
    a.expect(ReadableFileDescriptor(7)).then(callback);

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
        checkEqual(readFds, {7}, fdBound, "readFds");
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        readFds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
