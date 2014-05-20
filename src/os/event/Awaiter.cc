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
#include "Awaiter.hh"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <tuple>
#include <utility>
#include <vector>
#include "async/Future.hh"
#include "async/Promise.hh"
#include "os/Api.hh"
#include "os/event/Trigger.hh"

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createPromiseFuturePair;

using TimePoint = sesh::os::Api::SteadyClockTime;
using Clock = TimePoint::clock;

namespace sesh {
namespace os {
namespace event {

namespace {

Timeout timeoutOrMax(const Trigger &t) noexcept {
    if (t.index() == Trigger::index<Timeout>())
        return t.value<Timeout>();
    else
        return Timeout(Timeout::Interval::max());
}

/** @param ts non-empty trigger set. */
Timeout minTimeout(const std::vector<Trigger> &ts) {
    auto c = [](const Trigger &l, const Trigger &r) {
        return timeoutOrMax(l) < timeoutOrMax(r);
    };
    auto i = std::min_element(ts.begin(), ts.end(), c);
    assert(i != ts.end());
    return timeoutOrMax(*i);
}

class PendingEvent {

private:

    std::vector<Trigger> mTriggers;
    Promise<Trigger> mPromise;

public:

    PendingEvent(std::vector<Trigger> &&triggers, Promise<Trigger> &&promise);

    void fireTimeout();

}; // class OrderedTrigger

class AwaiterImpl : public Awaiter {

public:

    using TimeLimit = TimePoint;

private:

    const Api &mApi;
    std::multimap<TimeLimit, PendingEvent> mPendingEvents;

    Future<Trigger> expectImpl(std::vector<Trigger> &&triggers) final override;

public:

    explicit AwaiterImpl(const Api &);

    void awaitEvents() final override;

}; // class AwaiterImpl

PendingEvent::PendingEvent(
        std::vector<Trigger> &&triggers,
        Promise<Trigger> &&promise) :
        mTriggers(std::move(triggers)),
        mPromise(std::move(promise)) { }

void PendingEvent::fireTimeout() {
    std::move(mPromise).setResult(minTimeout(mTriggers));
}

AwaiterImpl::AwaiterImpl(const Api &api) :
        mApi(api), mPendingEvents() { }

TimePoint timeLimit(const Api &api, Timeout timeout) {
    if (timeout.interval() < Timeout::Interval::zero())
        timeout = Timeout(Timeout::Interval::zero());

    if (timeout.interval() == Timeout::Interval::max())
        return TimePoint::max();

    TimePoint now = api.steadyClockNow();
    if (now > TimePoint::max() - timeout.interval())
        return TimePoint::max();
    return now + timeout.interval();
}

Future<Trigger> AwaiterImpl::expectImpl(
        std::vector<Trigger> &&triggers) {
    auto promiseAndFuture = createPromiseFuturePair<Trigger>();
    if (triggers.empty())
        return std::move(promiseAndFuture.second);

    mPendingEvents.emplace(
            std::piecewise_construct,
            std::make_tuple(timeLimit(mApi, minTimeout(triggers))),
            std::forward_as_tuple(
                    std::move(triggers),
                    std::move(promiseAndFuture.first)));
    return std::move(promiseAndFuture.second);
}

void AwaiterImpl::awaitEvents() {
    while (!mPendingEvents.empty()) {
        auto i = mPendingEvents.begin();
        TimePoint nextTimeLimit = i->first;
        TimePoint now = mApi.steadyClockNow();
        if (nextTimeLimit <= now) {
            i->second.fireTimeout();
            mPendingEvents.erase(i);
            continue;
        }

        TimePoint::duration timeout;
        if (nextTimeLimit < TimePoint::max())
            timeout = nextTimeLimit - now;
        else
            timeout = TimePoint::duration(-1);
        mApi.pselect(0, nullptr, nullptr, nullptr, timeout, nullptr);
        //FIXME
    }
}

} // namespace

std::unique_ptr<Awaiter> createAwaiter(const Api &api) {
    return std::unique_ptr<Awaiter>(new AwaiterImpl(api));
}

} // namespace event
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
