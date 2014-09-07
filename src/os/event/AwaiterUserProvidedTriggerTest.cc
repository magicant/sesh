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
#include <memory>
#include <utility>
#include "async/future.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/PselectApi.hh"
#include "os/event/Trigger.hh"
#include "os/event/UserProvidedTrigger.hh"
#include "os/signaling/HandlerConfigurationApiTestHelper.hh"

namespace {

using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future_of;
using sesh::async::make_promise_future_pair;
using sesh::common::trial;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::Trigger;
using sesh::os::event::UserProvidedTrigger;
using sesh::os::signaling::HandlerConfigurationApiDummy;

using TimePoint = sesh::os::event::PselectApi::steady_clock_time;

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one user-provided trigger (successful future)") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;

    std::shared_ptr<void> result = std::make_shared<int>(1);
    future<Trigger> f = a.expect(UserProvidedTrigger(make_future_of(result)));
    std::move(f).then([this, &result](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == Trigger::tag<UserProvidedTrigger>());
        CHECK(t->value<UserProvidedTrigger>().result() == result);
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: one user-provided trigger (failed future)") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;

    future<Trigger> f = a.expect(UserProvidedTrigger(
                make_failed_future_of<std::shared_ptr<void>>(7)));
    std::move(f).then([this](trial<Trigger> &&t) {
        try {
            *t;
        } catch (int i) {
            CHECK(i == 7);
            mutable_steady_clock_now() += std::chrono::seconds(2);
        }
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two user-provided triggers in one trigger set") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutable_steady_clock_now() = startTime;

    using UPT = UserProvidedTrigger;
    std::shared_ptr<void> result = std::make_shared<int>(2);
    future<Trigger> f = a.expect(
            UPT(make_promise_future_pair<UPT::Result>().second),
            UPT(make_future_of(result)));
    std::move(f).then([this, &result](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == Trigger::tag<UserProvidedTrigger>());
        CHECK(t->value<UserProvidedTrigger>().result() == result);
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.awaitEvents();
    CHECK(steady_clock_now() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<HandlerConfigurationApiDummy>,
        "Awaiter: two user-provided triggers in two trigger sets") {
    std::shared_ptr<void> expected = std::make_shared<int>(0);
    auto f1 = a.expect(UserProvidedTrigger(make_future_of(expected)));
    auto f2 = std::move(f1).map([this](Trigger &&t) -> std::shared_ptr<void> {
        REQUIRE(t.tag() == Trigger::tag<UserProvidedTrigger>());
        return std::move(t.value<UserProvidedTrigger>().result());
    });
    auto f3 = a.expect(UserProvidedTrigger(std::move(f2)));
    std::shared_ptr<void> actual;
    std::move(f3).then([&actual](trial<Trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == Trigger::tag<UserProvidedTrigger>());
        actual = t->value<UserProvidedTrigger>().result();
    });

    a.awaitEvents();
    CHECK(actual.get() == expected.get());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
