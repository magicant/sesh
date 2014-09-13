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
#include "async/future.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "common/variant.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/error_file_descriptor.hh"
#include "os/event/pselect_api.hh"
#include "os/event/readable_file_descriptor.hh"
#include "os/event/signal.hh"
#include "os/event/timeout.hh"
#include "os/event/trigger.hh"
#include "os/event/user_provided_trigger.hh"
#include "os/event/writable_file_descriptor.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::future;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::error_file_descriptor;
using sesh::os::event::readable_file_descriptor;
using sesh::os::event::signal;
using sesh::os::event::timeout;
using sesh::os::event::trigger;
using sesh::os::event::user_provided_trigger;
using sesh::os::event::writable_file_descriptor;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::handler_configuration_api_dummy;
using sesh::os::signaling::SignalNumberSet;

using time_point = sesh::os::event::pselect_api::steady_clock_time;
using TriggerFileDescriptor = variant<
        readable_file_descriptor,
        writable_file_descriptor,
        error_file_descriptor>;

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: one trigger set containing readable and writable FDs") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            readable_file_descriptor(3), writable_file_descriptor(3));
    std::move(f).then([this, start_time](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        switch (t->tag()) {
        case trigger::tag<readable_file_descriptor>():
            CHECK(t->value<readable_file_descriptor>().value() == 3);
            break;
        case trigger::tag<writable_file_descriptor>():
            CHECK(t->value<writable_file_descriptor>().value() == 3);
            break;
        case trigger::tag<error_file_descriptor>():
        case trigger::tag<timeout>():
        case trigger::tag<signal>():
        case trigger::tag<user_provided_trigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_equal(read_fds, {3}, fd_bound, "read_fds");
        check_equal(write_fds, {3}, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: one trigger set containing readable and error FDs") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            readable_file_descriptor(3), error_file_descriptor(3));
    std::move(f).then([this, start_time](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        switch (t->tag()) {
        case trigger::tag<readable_file_descriptor>():
            CHECK(t->value<readable_file_descriptor>().value() == 3);
            break;
        case trigger::tag<error_file_descriptor>():
            CHECK(t->value<error_file_descriptor>().value() == 3);
            break;
        case trigger::tag<writable_file_descriptor>():
        case trigger::tag<timeout>():
        case trigger::tag<signal>():
        case trigger::tag<user_provided_trigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_equal(read_fds, {3}, fd_bound, "read_fds");
        check_empty(write_fds, fd_bound, "write_fds");
        check_equal(error_fds, {3}, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: one trigger set containing writable and error FDs") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            writable_file_descriptor(3), error_file_descriptor(3));
    std::move(f).then([this, start_time](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        switch (t->tag()) {
        case trigger::tag<writable_file_descriptor>():
            CHECK(t->value<writable_file_descriptor>().value() == 3);
            break;
        case trigger::tag<error_file_descriptor>():
            CHECK(t->value<error_file_descriptor>().value() == 3);
            break;
        case trigger::tag<readable_file_descriptor>():
        case trigger::tag<timeout>():
        case trigger::tag<signal>():
        case trigger::tag<user_provided_trigger>():
            FAIL("tag=" << t->tag());
            break;
        }
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(5));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds");
        check_equal(write_fds, {3}, fd_bound, "write_fds");
        check_equal(error_fds, {3}, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two trigger sets containing readable and writable FDs") {
    auto start_time = time_point(std::chrono::seconds(10000));
    mutable_steady_clock_now() = start_time;

    bool callback1Called = false, callback2Called = false;
    a.expect(readable_file_descriptor(2)).then(
            [this, start_time, &callback1Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<readable_file_descriptor>());
        CHECK(t->value<readable_file_descriptor>().value() == 2);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(writable_file_descriptor(3)).then(
            [this, start_time, &callback2Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t->value<writable_file_descriptor>().value() == 3);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, start_time](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutable_steady_clock_now() = start_time + std::chrono::seconds(10);
        return std::error_code();
    };
    a.await_events();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two trigger sets containing writable and error FDs") {
    auto start_time = time_point(std::chrono::seconds(10000));
    mutable_steady_clock_now() = start_time;

    bool callback1Called = false, callback2Called = false;
    a.expect(writable_file_descriptor(3)).then(
            [this, start_time, &callback1Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t->value<writable_file_descriptor>().value() == 3);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(error_file_descriptor(2)).then(
            [this, start_time, &callback2Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 2);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, start_time](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutable_steady_clock_now() = start_time + std::chrono::seconds(10);
        return std::error_code();
    };
    a.await_events();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two trigger sets containing readable and error FDs") {
    auto start_time = time_point(std::chrono::seconds(10000));
    mutable_steady_clock_now() = start_time;

    bool callback1Called = false, callback2Called = false;
    a.expect(readable_file_descriptor(2)).then(
            [this, start_time, &callback1Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<readable_file_descriptor>());
        CHECK(t->value<readable_file_descriptor>().value() == 2);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback1Called = true;
    });
    a.expect(error_file_descriptor(3)).then(
            [this, start_time, &callback2Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<error_file_descriptor>());
        CHECK(t->value<error_file_descriptor>().value() == 3);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(10));
        callback2Called = true;
    });

    implementation() = [this, start_time](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        mutable_steady_clock_now() = start_time + std::chrono::seconds(10);
        return std::error_code();
    };
    a.await_events();
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: awaiting max readable FD") {
    auto max = file_descriptor_set_impl::MAX_VALUE;
    bool callback_called = false;
    a.expect(readable_file_descriptor(max)).then(
            [this, max, &callback_called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<readable_file_descriptor>());
        CHECK(t->value<readable_file_descriptor>().value() == max);
        callback_called = true;
    });

    implementation() = [](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        return std::error_code();
    };
    a.await_events();
    CHECK(callback_called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: domain error from FD set") {
    auto max = file_descriptor_set_impl::MAX_VALUE;
    bool callback_called = false;
    a.expect(readable_file_descriptor(max + 1)).then(
            [this, &callback_called](trial<trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &) {
            callback_called = true;
        }
    });

    a.await_events();
    CHECK(callback_called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
