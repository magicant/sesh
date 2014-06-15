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
#include "async/Future.hh"
#include "common/Try.hh"
#include "os/Api.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/Trigger.hh"
#include "os/event/UserProvidedTrigger.hh"

namespace {

using sesh::async::Future;
using sesh::async::createFailedFutureOf;
using sesh::async::createFutureOf;
using sesh::common::Try;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::PselectAndNowApiStub;
using sesh::os::event::Trigger;
using sesh::os::event::UserProvidedTrigger;

using TimePoint = sesh::os::Api::SteadyClockTime;

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: one user-provided trigger (successful future)") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;

    std::shared_ptr<void> result = std::make_shared<int>(1);
    Future<Trigger> f = a.expect(UserProvidedTrigger(createFutureOf(result)));
    std::move(f).setCallback([this, &result](Try<Trigger> &&t) {
        REQUIRE(t.hasValue());
        REQUIRE(t->index() == Trigger::index<UserProvidedTrigger>());
        CHECK(t->value<UserProvidedTrigger>().result() == result);
        mutableSteadyClockNow() += std::chrono::seconds(2);
    });

    mutableSteadyClockNow() += std::chrono::seconds(10);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: one user-provided trigger (failed future)") {
    auto startTime = TimePoint(std::chrono::seconds(0));
    mutableSteadyClockNow() = startTime;

    Future<Trigger> f = a.expect(UserProvidedTrigger(
                createFailedFutureOf<std::shared_ptr<void>>(7)));
    std::move(f).setCallback([this](Try<Trigger> &&t) {
        try {
            *t;
        } catch (int i) {
            CHECK(i == 7);
            mutableSteadyClockNow() += std::chrono::seconds(2);
        }
    });

    mutableSteadyClockNow() += std::chrono::seconds(10);
    a.awaitEvents();
    CHECK(steadyClockNow() == startTime + std::chrono::seconds(12));
}

// TODO one user-provided trigger (result)
// TODO two user-provided triggers in one trigger set
// TODO two user-provided triggers in two trigger sets

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
