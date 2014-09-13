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
#include "async/future.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/awaiter.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/pselect_api.hh"
#include "os/event/signal.hh"
#include "os/event/trigger.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::async::future;
using sesh::common::trial;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::signal;
using sesh::os::event::trigger;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::HandlerConfigurationApiFake;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: one signal in one trigger set") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(signal(3));
    std::move(f).then([this](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == trigger::tag<signal>());
        CHECK(t->value<signal>().number() == 3);
        mutable_steady_clock_now() += std::chrono::seconds(2);
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
        check_empty(write_fds, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        if (signal_mask != nullptr)
            CHECK_FALSE(signal_mask->test(3));

        Action &a = actions().at(3);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutable_steady_clock_now() += std::chrono::seconds(7);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: irrelevant signals are masked") {
    a.expect(signal(3));

    signalMask().set(2);
    signalMask().set(5);
    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *mask_while_awaiting) -> std::error_code {
        if (mask_while_awaiting != nullptr)
            CHECK_FALSE(mask_while_awaiting->test(3));
        CHECK(signalMask().test(2));
        CHECK(signalMask().test(5));

        Action &a = actions().at(3);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(3);

        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    a.await_events();
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: two signals in one trigger set") {
    auto start_time = time_point(std::chrono::seconds(100));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(signal(2), signal(6));
    std::move(f).then([this](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == trigger::tag<signal>());
        CHECK(t->value<signal>().number() == 6);
        mutable_steady_clock_now() += std::chrono::seconds(2);
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
        check_empty(write_fds, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        if (signal_mask != nullptr) {
            CHECK_FALSE(signal_mask->test(3));
            CHECK_FALSE(signal_mask->test(6));
        }

        Action &a2 = actions().at(2);
        REQUIRE(a2.tag() == Action::tag<sesh_osapi_signal_handler *>());

        Action &a6 = actions().at(6);
        REQUIRE(a6.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a6.value<sesh_osapi_signal_handler *>()(6);

        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutable_steady_clock_now() += std::chrono::seconds(7);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: same signal in two trigger sets") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    for (unsigned i = 0; i < 2; ++i) {
        a.expect(signal(1)).then([this](trial<trigger> &&t) {
            REQUIRE(t.has_value());
            REQUIRE(t->tag() == trigger::tag<signal>());
            CHECK(t->value<signal>().number() == 1);
            mutable_steady_clock_now() += std::chrono::seconds(1);
        });
    }

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds");
        check_empty(write_fds, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        if (signal_mask != nullptr)
            CHECK_FALSE(signal_mask->test(1));

        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutable_steady_clock_now() += std::chrono::seconds(7);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: different signals in two trigger sets: fired at a time") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    for (SignalNumber sn : {1, 2}) {
        a.expect(signal(sn)).then([this, sn](trial<trigger> &&t) {
            REQUIRE(t.has_value());
            REQUIRE(t->tag() == trigger::tag<signal>());
            CHECK(t->value<signal>().number() == sn);
            mutable_steady_clock_now() += std::chrono::seconds(1);
        });
    }

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *signal_mask) -> std::error_code {
        if (signal_mask != nullptr) {
            CHECK_FALSE(signal_mask->test(1));
            CHECK_FALSE(signal_mask->test(2));
        }

        Action &a1 = actions().at(1);
        REQUIRE(a1.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a1.value<sesh_osapi_signal_handler *>()(1);

        Action &a2 = actions().at(2);
        REQUIRE(a2.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a2.value<sesh_osapi_signal_handler *>()(2);

        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    mutable_steady_clock_now() += std::chrono::seconds(7);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: different signals in two trigger sets: "
        "fired intermittently") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    for (SignalNumber sn : {1, 2}) {
        a.expect(signal(sn)).then([this, sn](trial<trigger> &&t) {
            REQUIRE(t.has_value());
            REQUIRE(t->tag() == trigger::tag<signal>());
            CHECK(t->value<signal>().number() == sn);
            mutable_steady_clock_now() += std::chrono::seconds(1);
        });
    }

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *signal_mask) -> std::error_code {
        if (signal_mask != nullptr) {
            CHECK_FALSE(signal_mask->test(1));
            CHECK_FALSE(signal_mask->test(2));
        }

        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        mutable_steady_clock_now() += std::chrono::seconds(3);
        implementation() = [this](
                const pselect_api_stub &,
                file_descriptor::value_type,
                file_descriptor_set *,
                file_descriptor_set *,
                file_descriptor_set *,
                std::chrono::nanoseconds,
                const SignalNumberSet *signal_mask) -> std::error_code {
            if (signal_mask != nullptr)
                CHECK_FALSE(signal_mask->test(2));

            Action &a = actions().at(2);
            REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
            a.value<sesh_osapi_signal_handler *>()(2);

            mutable_steady_clock_now() += std::chrono::seconds(3);
            implementation() = nullptr;
            return std::make_error_code(std::errc::interrupted);
        };
        return std::make_error_code(std::errc::interrupted);
    };
    mutable_steady_clock_now() += std::chrono::seconds(7);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(15));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiFake>,
        "Awaiter: signal handler is reset after event fired") {
    a.expect(signal(1));

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const SignalNumberSet *) -> std::error_code {
        Action &a = actions().at(1);
        REQUIRE(a.tag() == Action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        implementation() = nullptr;
        return std::make_error_code(std::errc::interrupted);
    };
    a.await_events();

    Action &a = actions().at(1);
    CHECK(a.tag() == Action::tag<Default>());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
