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
#include <exception>
#include <system_error>
#include <utility>
#include "async/future.hh"
#include "catch.hpp"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/pselect_api.hh"
#include "os/event/readable_file_descriptor.hh"
#include "os/event/signal.hh"
#include "os/event/timeout.hh"
#include "os/event/trigger.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/signal_number_set.hh"

/*
namespace std {

template<typename Char, typename Traits, typename R, typename P>
std::ostream &operator<<(
        std::basic_ostream<Char, Traits> &s, std::chrono::duration<R, P> d) {
    s << std::chrono::duration_cast<std::chrono::nanoseconds>(d).count() <<
            " ns";
    return s;
}

template<typename Char, typename Traits, typename C, typename D>
std::ostream &operator<<(
        std::basic_ostream<Char, Traits> &s, std::chrono::time_point<C, D> t) {
    return s << t.time_since_epoch() << " since epoch";
}

}
*/

namespace {

using sesh::common::trial;
using sesh::async::future;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::readable_file_descriptor;
using sesh::os::event::signal;
using sesh::os::event::timeout;
using sesh::os::event::trigger;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::handler_configuration_api_dummy;
using sesh::os::signaling::handler_configuration_api_fake;
using sesh::os::signaling::signal_number_set;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: doesn't wait if no events are pending") {
    a.await_events();
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: does nothing for empty trigger set") {
    future<trigger> f = a.expect();
    std::move(f).then([](trial<trigger> &&) { FAIL("callback called"); });
    a.await_events();
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: timeout with FD trigger in same set") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            timeout(std::chrono::seconds(5)), readable_file_descriptor(3));
    std::move(f).then([this, start_time](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == t->tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(5));
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
        check_equal(read_fds, {3}, fd_bound, "read_fds");
        check_empty(write_fds, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout == std::chrono::seconds(4));
        CHECK(signal_mask == nullptr);
        read_fds->reset();
        mutable_steady_clock_now() += std::chrono::seconds(4);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(7));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: FD trigger with timeout in same set") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            timeout(std::chrono::seconds(10)), readable_file_descriptor(3));
    std::move(f).then([this, start_time](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == t->tag<readable_file_descriptor>());
        CHECK(t->value<readable_file_descriptor>().value() == 3);
        mutable_steady_clock_now() += std::chrono::seconds(4);
    });

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const signal_number_set *signal_mask) -> std::error_code {
        check_equal(read_fds, {3}, fd_bound, "read_fds");
        check_empty(write_fds, fd_bound, "write_fds");
        check_empty(error_fds, fd_bound, "error_fds");
        CHECK(timeout == std::chrono::seconds(9));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(2);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(7));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_fake>,
        "Awaiter: signal handler is reset after event fired (with timeout)") {
    a.expect(timeout(std::chrono::seconds(1)), signal(1));

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const signal_number_set *) -> std::error_code {
        signal_action &a = actions().at(1);
        CHECK(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

        mutable_steady_clock_now() += std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    a.await_events();

    signal_action &a = actions().at(1);
    CHECK(a.tag() == signal_action::tag<default_action>());
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: setting timeout from domain error") {
    auto fd = readable_file_descriptor(file_descriptor_set_impl::max + 1);
    bool called = false;
    a.expect(fd).wrap().recover([this](std::exception_ptr) {
        return a.expect(timeout(std::chrono::seconds(0)));
    }).unwrap().then([&](trial<trigger> &&) {
        called = true;
    });
    a.await_events();
    CHECK(called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_fake>,
        "Awaiter: ignores FD set if pselect failed") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;

    bool called = false;
    auto f = a.expect(
            timeout(std::chrono::seconds(1)), readable_file_descriptor(0));
    std::move(f).then([&called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == t->tag<timeout>());
        called = true;
    });

    a.expect(signal(1));

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type,
            file_descriptor_set *,
            file_descriptor_set *,
            file_descriptor_set *,
            std::chrono::nanoseconds,
            const signal_number_set *) -> std::error_code {
        signal_action &a = actions().at(1);
        REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());
        a.value<sesh_osapi_signal_handler *>()(1);

        implementation() = [this](
                const pselect_api_stub &,
                file_descriptor::value_type,
                file_descriptor_set *read_fds,
                file_descriptor_set *,
                file_descriptor_set *,
                std::chrono::nanoseconds,
                const signal_number_set *) -> std::error_code {
            if (read_fds != nullptr)
                read_fds->reset();
            mutable_steady_clock_now() += std::chrono::seconds(1);
            return std::error_code();
        };
        return std::make_error_code(std::errc::interrupted);
    };

    a.await_events();
    CHECK(called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
