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
#include "os/event/Timeout.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"
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
using sesh::os::event::Timeout;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfigurationApiDummy;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::pselect_api::steady_clock_time;

template<int durationInSecondsInt>
class TimeoutTest :
        protected awaiter_test_fixture<HandlerConfigurationApiDummy> {

protected:

    constexpr static std::chrono::seconds duration() noexcept {
        return std::chrono::seconds(durationInSecondsInt);
    }

public:

    TimeoutTest();

}; // class TimeoutTest

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: timeout 0") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(Timeout(std::chrono::seconds(0)));
    bool callbackCalled = false;
    std::move(f).then([this, startTime, &callbackCalled](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(0));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(1));
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds");
        check_empty(writeFds, fdBound, "writeFds");
        check_empty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::seconds(0));
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(1));
    CHECK(callbackCalled);
}

template<int durationInSecondsInt>
TimeoutTest<durationInSecondsInt>::TimeoutTest() {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(Timeout(duration()));
    bool callbackCalled = false;
    std::move(f).then([this, startTime, &callbackCalled](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == duration());
        CHECK(steady_clock_now() == startTime + duration());
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds");
        check_empty(writeFds, fdBound, "writeFds");
        check_empty(errorFds, fdBound, "errorFds");
        CHECK(timeout == duration() - std::chrono::seconds(1));
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += duration() - std::chrono::seconds(1);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == startTime + duration());
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(TimeoutTest<1>, "Awaiter: timeout 1") { }

TEST_CASE_METHOD(TimeoutTest<2>, "Awaiter: timeout 2") { }

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: negative timeout") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(Timeout(std::chrono::seconds(-10)));
    bool callbackCalled = false;
    std::move(f).then([this, startTime, &callbackCalled](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(-10));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(1));
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds");
        check_empty(writeFds, fdBound, "writeFds");
        check_empty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::nanoseconds::zero());
        CHECK(signalMask == nullptr);
        implementation() = nullptr;
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(1));
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: duplicate timeouts in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(-100));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(
            Timeout(std::chrono::seconds(10)),
            Timeout(std::chrono::seconds(5)),
            Timeout(std::chrono::seconds(20)));
    bool callbackCalled = false;
    std::move(f).then([this, startTime, &callbackCalled](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(5));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(5));
        callbackCalled = true;
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds");
        check_empty(writeFds, fdBound, "writeFds");
        check_empty(errorFds, fdBound, "errorFds");
        CHECK(timeout == std::chrono::seconds(5));
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(5);
        implementation() = nullptr;
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(5));
    CHECK(callbackCalled);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two simultaneous timeouts") {
    auto startTime = TimePoint(std::chrono::seconds(1000));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f1 = a.expect(Timeout(std::chrono::seconds(10)));
    bool callback1Called = false;
    std::move(f1).then(
            [this, startTime, &callback1Called](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(10));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(11));
        callback1Called = true;
    });
    CHECK_FALSE(callback1Called);

    mutable_steady_clock_now() = startTime + std::chrono::seconds(1);
    future<Trigger> f2 = a.expect(Timeout(std::chrono::seconds(29)));
    bool callback2Called = false;
    std::move(f2).then(
            [this, startTime, &callback2Called](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(29));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(32));
        callback2Called = true;
    });
    CHECK_FALSE(callback2Called);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds 1");
        check_empty(writeFds, fdBound, "writeFds 1");
        check_empty(errorFds, fdBound, "errorFds 1");
        CHECK(timeout == std::chrono::seconds(9));
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(10);

        implementation() = [this](
                const pselect_api_stub &,
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) -> std::error_code {
            check_empty(readFds, fdBound, "readFds 2");
            check_empty(writeFds, fdBound, "writeFds 2");
            check_empty(errorFds, fdBound, "errorFds 2");
            CHECK(timeout == std::chrono::seconds(19));
            CHECK(signalMask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(21);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(32));
    CHECK(callback1Called);
    CHECK(callback2Called);
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two successive timeouts") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;
    future<Trigger> f = a.expect(Timeout(std::chrono::seconds(100)));
    bool callbackCalled = false;
    std::move(f).map([this, startTime](Trigger &&t) -> future<Trigger> {
        CHECK(t.tag() == Trigger::tag<Timeout>());
        CHECK(t.value<Timeout>().interval() == std::chrono::seconds(100));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(102));

        future<Trigger> f2 = a.expect(Timeout(std::chrono::seconds(8)));
        mutable_steady_clock_now() += std::chrono::seconds(1);
        return f2;
    }).unwrap().then([this, startTime, &callbackCalled](trial<Trigger> &&t) {
        CHECK(t->tag() == Trigger::tag<Timeout>());
        CHECK(t->value<Timeout>().interval() == std::chrono::seconds(8));
        CHECK(steady_clock_now() == startTime + std::chrono::seconds(113));

        callbackCalled = true;
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });
    CHECK_FALSE(callbackCalled);

    implementation() = [this](
            const pselect_api_stub &,
            FileDescriptor::Value fdBound,
            FileDescriptorSet *readFds,
            FileDescriptorSet *writeFds,
            FileDescriptorSet *errorFds,
            std::chrono::nanoseconds timeout,
            const SignalNumberSet *signalMask) -> std::error_code {
        check_empty(readFds, fdBound, "readFds 1");
        check_empty(writeFds, fdBound, "writeFds 1");
        check_empty(errorFds, fdBound, "errorFds 1");
        CHECK(timeout == std::chrono::seconds(99));
        CHECK(signalMask == nullptr);
        mutable_steady_clock_now() += std::chrono::seconds(101);

        implementation() = [this](
                const pselect_api_stub &,
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) -> std::error_code {
            check_empty(readFds, fdBound, "readFds 2");
            check_empty(writeFds, fdBound, "writeFds 2");
            check_empty(errorFds, fdBound, "errorFds 2");
            CHECK(timeout == std::chrono::seconds(7));
            CHECK(signalMask == nullptr);
            mutable_steady_clock_now() += std::chrono::seconds(10);
            implementation() = nullptr;
            return std::error_code();
        };
        return std::error_code();
    };
    mutable_steady_clock_now() += std::chrono::seconds(1);
    a.await_events();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(115));
    CHECK(callbackCalled);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
