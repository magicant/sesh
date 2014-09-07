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
#include "os/event/awaiter_test_helper.hh"
#include "os/event/error_file_descriptor.hh"
#include "os/event/PselectApi.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::future;
using sesh::common::trial;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::error_file_descriptor;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiDummy;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::steady_clock_time;

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: awaiting single error FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(error_file_descriptor(4));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 4);
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {4}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing different error FDs: "
        "pselect returning single FD") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            error_file_descriptor(2), error_file_descriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 0);
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {0, 2}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        errorFds->reset(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: one trigger set containing different error FDs: "
        "pselect returning all FDs") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            error_file_descriptor(2), error_file_descriptor(0));
    std::move(f).then([this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        auto fd = t->value<error_file_descriptor>().value();
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
        checkEmpty(writeFds, fdBound, "writeFds");
        checkEqual(errorFds, {0, 2}, fdBound, "errorFds");
        CHECK(timeout.count() < 0);
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing different error FDs") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    a.expect(error_file_descriptor(1)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 1);
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(9));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });
    a.expect(error_file_descriptor(3)).then(
            [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 3);
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
        checkEmpty(writeFds, fdBound, "writeFds 1");
        checkEqual(errorFds, {1, 3}, fdBound, "errorFds 1");
        errorFds->reset(3);
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
            checkEmpty(writeFds, fdBound, "writeFds 2");
            checkEqual(errorFds, {3}, fdBound, "errorFds 2");
            CHECK(timeout.count() < 0);
            CHECK(signalMask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(18);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(30));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two trigger sets containing same error FD") {
    auto startTime = TimePoint(std::chrono::seconds(10000));
    mutable_steady_clock_now() = startTime;

    auto callback = [this, startTime](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 7);
        mutable_steady_clock_now() += std::chrono::seconds(1);
    };
    a.expect(error_file_descriptor(7)).then(callback);
    a.expect(error_file_descriptor(7)).then(callback);

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
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
