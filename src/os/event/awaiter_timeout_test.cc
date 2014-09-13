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
#include <set>
#include <system_error>
#include <utility>
#include "async/future.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/awaiter_test_helper.hh"
#include "os/event/pselect_api.hh"
#include "os/event/timeout.hh"
#include "os/event/trigger.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/SignalNumberSet.hh"

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

using sesh::async::future;
using sesh::common::trial;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::timeout;
using sesh::os::event::trigger;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::handler_configuration_api_dummy;
using sesh::os::signaling::SignalNumberSet;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

template<int DurationInSecondsInt>
class TimeoutTest :
        protected awaiter_test_fixture<handler_configuration_api_dummy> {

protected:

    constexpr static std::chrono::seconds duration() noexcept {
        return std::chrono::seconds(DurationInSecondsInt);
    }

public:

    TimeoutTest();

}; // class TimeoutTest

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: timeout 0") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(timeout(std::chrono::seconds(0)));
    bool callback_called = false;
    std::move(f).then(
            [this, start_time, &callback_called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(0));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(1));
        callback_called = true;
    });
    CHECK_FALSE(callback_called);

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
        CHECK(timeout == std::chrono::seconds(0));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(1));
    CHECK(callback_called);
}

template<int DurationInSecondsInt>
TimeoutTest<DurationInSecondsInt>::TimeoutTest() {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(timeout(duration()));
    bool callback_called = false;
    std::move(f).then([
            this, start_time, &callback_called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == duration());
        CHECK(steady_clock_now() == start_time + duration());
        callback_called = true;
    });
    CHECK_FALSE(callback_called);

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
        CHECK(timeout == duration() - std::chrono::seconds(1));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += duration() - std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + duration());
    CHECK(callback_called);
}

TEST_CASE_METHOD(TimeoutTest<1>, "Awaiter: timeout 1") { }

TEST_CASE_METHOD(TimeoutTest<2>, "Awaiter: timeout 2") { }

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: negative timeout") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(timeout(std::chrono::seconds(-10)));
    bool callback_called = false;
    std::move(f).then([
            this, start_time, &callback_called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(-10));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(1));
        callback_called = true;
    });
    CHECK_FALSE(callback_called);

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
        CHECK(timeout == std::chrono::nanoseconds::zero());
        CHECK(signal_mask == nullptr);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(1));
    CHECK(callback_called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: duplicate timeouts in one trigger set") {
    auto start_time = time_point(std::chrono::seconds(-100));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(
            timeout(std::chrono::seconds(10)),
            timeout(std::chrono::seconds(5)),
            timeout(std::chrono::seconds(20)));
    bool callback_called = false;
    std::move(f).then([
            this, start_time, &callback_called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(5));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(5));
        callback_called = true;
    });
    CHECK_FALSE(callback_called);

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
        CHECK(timeout == std::chrono::seconds(5));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(5);
        implementation() = nullptr;
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(5));
    CHECK(callback_called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two simultaneous timeouts") {
    auto start_time = time_point(std::chrono::seconds(1000));
    mutable_steady_clock_now() = start_time;
    future<trigger> f1 = a.expect(timeout(std::chrono::seconds(10)));
    bool callback1Called = false;
    std::move(f1).then(
            [this, start_time, &callback1Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(10));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(11));
        callback1Called = true;
    });
    CHECK_FALSE(callback1Called);

    mutable_steady_clock_now() = start_time + std::chrono::seconds(1);
    future<trigger> f2 = a.expect(timeout(std::chrono::seconds(29)));
    bool callback2Called = false;
    std::move(f2).then(
            [this, start_time, &callback2Called](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(29));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(32));
        callback2Called = true;
    });
    CHECK_FALSE(callback2Called);

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds 1");
        check_empty(write_fds, fd_bound, "write_fds 1");
        check_empty(error_fds, fd_bound, "error_fds 1");
        CHECK(timeout == std::chrono::seconds(9));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(10);

        implementation() = [this](
                const pselect_api_stub &,
                file_descriptor::value_type fd_bound,
                file_descriptor_set *read_fds,
                file_descriptor_set *write_fds,
                file_descriptor_set *error_fds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signal_mask) -> std::error_code {
            check_empty(read_fds, fd_bound, "read_fds 2");
            check_empty(write_fds, fd_bound, "write_fds 2");
            check_empty(error_fds, fd_bound, "error_fds 2");
            CHECK(timeout == std::chrono::seconds(19));
            CHECK(signal_mask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(21);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(32));
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<handler_configuration_api_dummy>,
        "Awaiter: two successive timeouts") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;
    future<trigger> f = a.expect(timeout(std::chrono::seconds(100)));
    bool callback_called = false;
    std::move(f).map([this, start_time](trigger &&t) -> future<trigger> {
        CHECK(t.tag() == trigger::tag<timeout>());
        CHECK(t.value<timeout>().interval() == std::chrono::seconds(100));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(102));

        future<trigger> f2 = a.expect(timeout(std::chrono::seconds(8)));
        mutable_steady_clock_now() += std::chrono::seconds(1);
        return f2;
    }).unwrap().then([this, start_time, &callback_called](trial<trigger> &&t) {
        CHECK(t->tag() == trigger::tag<timeout>());
        CHECK(t->value<timeout>().interval() == std::chrono::seconds(8));
        CHECK(steady_clock_now() == start_time + std::chrono::seconds(113));

        callback_called = true;
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });
    CHECK_FALSE(callback_called);

    implementation() = [this](
            const pselect_api_stub &,
            file_descriptor::value_type fd_bound,
            file_descriptor_set *read_fds,
            file_descriptor_set *write_fds,
            file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signal_mask) -> std::error_code {
        check_empty(read_fds, fd_bound, "read_fds 1");
        check_empty(write_fds, fd_bound, "write_fds 1");
        check_empty(error_fds, fd_bound, "error_fds 1");
        CHECK(timeout == std::chrono::seconds(99));
        CHECK(signal_mask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(101);

        implementation() = [this](
                const pselect_api_stub &,
                file_descriptor::value_type fd_bound,
                file_descriptor_set *read_fds,
                file_descriptor_set *write_fds,
                file_descriptor_set *error_fds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signal_mask) -> std::error_code {
            check_empty(read_fds, fd_bound, "read_fds 2");
            check_empty(write_fds, fd_bound, "write_fds 2");
            check_empty(error_fds, fd_bound, "error_fds 2");
            CHECK(timeout == std::chrono::seconds(7));
            CHECK(signal_mask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(10);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(115));
    CHECK(callback_called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
