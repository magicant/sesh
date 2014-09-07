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
#include "os/event/Awaiter.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/PselectApi.hh"
#include "os/event/Trigger.hh"
#include "os/event/WritableFileDescriptor.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::future;
using sesh::common::trial;
using sesh::os::event::Awaiter;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::Trigger;
using sesh::os::event::WritableFileDescriptor;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiDummy;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::steady_clock_time;

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: awaiting single writable FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(WritableFileDescriptor(4));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 4);
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
        checkEmpty(readFds, fdBound, "readFds");
        checkEqual(writeFds, {4}, fdBound, "writeFds");
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
        "Awaiter: one trigger set containing different writable FDs: "
        "pselect returning single FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            WritableFileDescriptor(2), WritableFileDescriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 0);
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
        checkEmpty(readFds, fdBound, "readFds");
        checkEqual(writeFds, {0, 2}, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        writeFds->reset(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing different writable FDs: "
        "pselect returning all FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            WritableFileDescriptor(2), WritableFileDescriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        auto fd = t->value<WritableFileDescriptor>().value();
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
        checkEmpty(readFds, fdBound, "readFds");
        checkEqual(writeFds, {0, 2}, fdBound, "writeFds");
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
        "Awaiter: two trigger sets containing different writable FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    a.expect(WritableFileDescriptor(1)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 1);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(9));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });
    a.expect(WritableFileDescriptor(3)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 3);
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
        checkEmpty(readFds, fdBound, "readFds 1");
        checkEqual(writeFds, {1, 3}, fdBound, "writeFds 1");
        checkEmpty(errorFds, fdBound, "errorFds 1");
        writeFds->reset(3);
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
            checkEmpty(readFds, fdBound, "readFds 2");
            checkEqual(writeFds, {3}, fdBound, "writeFds 2");
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
        "Awaiter: two trigger sets containing same writable FD") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    auto callback = [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<WritableFileDescriptor>());
        CHECK(t->value<WritableFileDescriptor>().value() == 7);
        mutable_steady_clock_now() += std::chrono::seconds(1);
    };
    a.expect(WritableFileDescriptor(7)).then(callback);
    a.expect(WritableFileDescriptor(7)).then(callback);

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
        checkEqual(writeFds, {7}, fdBound, "writeFds");
        checkEmpty(errorFds, fdBound, "errorFds");
        writeFds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        return std::error_code();
    };
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
