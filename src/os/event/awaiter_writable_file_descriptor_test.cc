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

#include <chrono>
#include <iterator>
#include <set>
#include <system_error>
#include <utility>
#include "async/future.hh"
#include "async/future_test_helper.hh"
#include "catch.hpp"
#include "common/type_tag_test_helper.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/pselect_api.hh"
#include "os/event/trigger.hh"
#include "os/event/writable_file_descriptor.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/signal_number_set.hh"

namespace {

using sesh::async::future;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::trigger;
using sesh::os::event::writable_file_descriptor;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::handler_configuration_api_dummy;
using sesh::os::signaling::signal_number_set;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: awaiting single writable FD") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    expect_result(
            a.expect(writable_file_descriptor(4)),
            [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == 4);
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
            const signal_number_set *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds");
        check_equal(write_fds, {4}, fd_bound, "write_fds");
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
        "Awaiter: one trigger set containing different writable FDs: "
        "pselect returning single FD") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    expect_result(
            a.expect(
                writable_file_descriptor(2), writable_file_descriptor(0)),
            [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == 0);
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
            const signal_number_set *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds");
        check_equal(write_fds, {0, 2}, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(3);
        write_fds->reset(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(2);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(6));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: one trigger set containing different writable FDs: "
        "pselect returning all FDs") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    expect_result(
            a.expect(
                writable_file_descriptor(2), writable_file_descriptor(0)),
            [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        auto fd = t.value<writable_file_descriptor>().value();
        if (fd != 0)
            CHECK(fd == 2);
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
            const signal_number_set *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds");
        check_equal(write_fds, {0, 2}, fd_bound, "write_fds");
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
        "Awaiter: two trigger sets containing different writable FDs") {
    auto start_time = time_point(std::chrono::seconds(10000));
    mutable_steady_clock_now() = start_time;

    expect_result(
            a.expect(writable_file_descriptor(1)),
            [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == 1);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(9));
        mutable_steady_clock_now() += std::chrono::seconds(1);
    });
    expect_result(
            a.expect(writable_file_descriptor(3)),
            [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == 3);
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(28));
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const signal_number_set *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds 1");
        check_equal(write_fds, {1, 3}, fd_bound, "write_fds 1");
        check_empty(error_fds, fd_bound, "error_fds 1");
        write_fds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);

        mutable_steady_clock_now() += std::chrono::seconds(9);
        implementation() = [this](
                const pselect_api_stub &,
                file_descriptor::value_type fd_bound,
                file_descriptor_set *read_fds,
                file_descriptor_set *write_fds,
                file_descriptor_set *error_fds,
                std::chrono::nanoseconds timeout,
                const signal_number_set *signal_mask) -> std::error_code {
            check_empty(read_fds, fd_bound, "read_fds 2");
            check_equal(write_fds, {3}, fd_bound, "write_fds 2");
            check_empty(error_fds, fd_bound, "error_fds 2");
            CHECK(timeout.count() < 0);
            CHECK(signal_mask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(18);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(30));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two trigger sets containing same writable FD") {
    auto start_time = time_point(std::chrono::seconds(10000));
    mutable_steady_clock_now() = start_time;

    auto checker = [this, start_time](trigger &&t) {
        CHECK(t.tag() == trigger::tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == 7);
        mutable_steady_clock_now() += std::chrono::seconds(1);
    };
    expect_result(a.expect(writable_file_descriptor(7)), checker);
    expect_result(a.expect(writable_file_descriptor(7)), checker);

    unsigned count = 0;
    implementation() = [this, &count](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const signal_number_set *signal_mask) -> std::error_code {
        INFO(count);
        ++count;
        check_empty(read_fds, fd_bound, "read_fds");
        check_equal(write_fds, {7}, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        write_fds->reset(3);
        CHECK(timeout.count() < 0);
        CHECK(signal_mask == nullptr);
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(2));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
