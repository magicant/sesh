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
#include "os/event/awaiter_test_helper.hh"
#include "os/event/pselect_api.hh"
#include "os/event/trigger.hh"
#include "os/event/user_provided_trigger.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"

namespace {

using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future_of;
using sesh::async::make_promise_future_pair;
using sesh::common::trial;
using sesh::os::event::awaiter_test_fixture;
using sesh::os::event::trigger;
using sesh::os::event::user_provided_trigger;
using sesh::os::signaling::HandlerConfigurationApiDummy;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: one user-provided trigger (successful future)") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;

    std::shared_ptr<void> result = std::make_shared<int>(1);
    future<trigger> f =
            a.expect(user_provided_trigger(make_future_of(result)));
    std::move(f).then([this, &result](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == trigger::tag<user_provided_trigger>());
        CHECK(t->value<user_provided_trigger>().result() == result);
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: one user-provided trigger (failed future)") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;

    future<trigger> f = a.expect(user_provided_trigger(
                make_failed_future_of<std::shared_ptr<void>>(7)));
    std::move(f).then([this](trial<trigger> &&t) {
        try {
            *t;
        } catch (int i) {
            CHECK(i == 7);
            mutable_steady_clock_now() += std::chrono::seconds(2);
        }
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two user-provided triggers in one trigger set") {
    auto start_time = time_point(std::chrono::seconds(0));
    mutable_steady_clock_now() = start_time;

    using UPT = user_provided_trigger;
    std::shared_ptr<void> result = std::make_shared<int>(2);
    future<trigger> f = a.expect(
            UPT(make_promise_future_pair<UPT::result_type>().second),
            UPT(make_future_of(result)));
    std::move(f).then([this, &result](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == trigger::tag<user_provided_trigger>());
        CHECK(t->value<user_provided_trigger>().result() == result);
        mutable_steady_clock_now() += std::chrono::seconds(2);
    });

    mutable_steady_clock_now() += std::chrono::seconds(10);
    a.await_events();
    CHECK(steady_clock_now() == start_time + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        awaiter_test_fixture<HandlerConfigurationApiDummy>,
        "Awaiter: two user-provided triggers in two trigger sets") {
    std::shared_ptr<void> expected = std::make_shared<int>(0);
    auto f1 = a.expect(user_provided_trigger(make_future_of(expected)));
    auto f2 = std::move(f1).map([this](trigger &&t) -> std::shared_ptr<void> {
        REQUIRE(t.tag() == trigger::tag<user_provided_trigger>());
        return std::move(t.value<user_provided_trigger>().result());
    });
    auto f3 = a.expect(user_provided_trigger(std::move(f2)));
    std::shared_ptr<void> actual;
    std::move(f3).then([&actual](trial<trigger> &&t) {
        REQUIRE(t.has_value());
        REQUIRE(t->tag() == trigger::tag<user_provided_trigger>());
        actual = t->value<user_provided_trigger>().result();
    });

    a.await_events();
    CHECK(actual.get() == expected.get());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
