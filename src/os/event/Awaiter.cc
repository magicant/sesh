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
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include "async/Future.hh"
#include "async/Promise.hh"
#include "common/ContainerHelper.hh"
#include "helpermacros.h"
#include "os/Api.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createPromiseFuturePair;
using sesh::common::find_if;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;

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

struct FdSetTriple {
    FileDescriptor::Value bound = 0;
    std::unique_ptr<FileDescriptorSet> readFds, writeFds, errorFds;
};

class PendingEvent {

private:

    std::vector<Trigger> mTriggers;
    Promise<Trigger> mPromise;

public:

    PendingEvent(std::vector<Trigger> &&triggers, Promise<Trigger> &&promise);

    void addFdsToAwait(const Api &, FdSetTriple &) const;

    void fireTimeout();

    bool testConditionAndFire(const FdSetTriple &);

    void failWithCurrentException();

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

void addFd(
        const Api &api,
        std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd) {
    if (fds == nullptr)
        fds = api.createFileDescriptorSet();
    fds->set(fd);
}

void addFdToAwait(const Api &api, const Trigger &t, FdSetTriple &fds) {
    switch (t.index()) {
    case Trigger::index<ReadableFileDescriptor>():
        addFd(api, fds.readFds, t.value<ReadableFileDescriptor>().value());
        fds.bound = std::max(
                fds.bound, t.value<ReadableFileDescriptor>().value() + 1);
        break;
    case Trigger::index<WritableFileDescriptor>():
        addFd(api, fds.writeFds, t.value<WritableFileDescriptor>().value());
        fds.bound = std::max(
                fds.bound, t.value<WritableFileDescriptor>().value() + 1);
        break;
    case Trigger::index<ErrorFileDescriptor>():
        addFd(api, fds.errorFds, t.value<ErrorFileDescriptor>().value());
        fds.bound = std::max(
                fds.bound, t.value<ErrorFileDescriptor>().value() + 1);
        break;
    case Trigger::index<Timeout>():
    case Trigger::index<Signal>():
    case Trigger::index<UserProvidedTrigger>():
        break;
    }
}

void PendingEvent::addFdsToAwait(const Api &api, FdSetTriple &fds) const {
    for (const Trigger &t : mTriggers)
        addFdToAwait(api, t, fds);
}

void PendingEvent::fireTimeout() {
    std::move(mPromise).setResult(minTimeout(mTriggers));
}

bool contains(
        const std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd) {
    return fds != nullptr && fds->test(fd);
}

bool testCondition(const Trigger &t, const FdSetTriple &fds) {
    switch (t.index()) {
    case Trigger::index<Timeout>():
        return false;
    case Trigger::index<ReadableFileDescriptor>():
        return contains(
                fds.readFds, t.value<ReadableFileDescriptor>().value());
    case Trigger::index<WritableFileDescriptor>():
        return contains(
                fds.writeFds, t.value<WritableFileDescriptor>().value());
    case Trigger::index<ErrorFileDescriptor>():
        return contains(
                fds.errorFds, t.value<ErrorFileDescriptor>().value());
    case Trigger::index<Signal>():
    case Trigger::index<UserProvidedTrigger>():
        return false; // FIXME
    }
    UNREACHABLE();
}

bool PendingEvent::testConditionAndFire(const FdSetTriple &fds) {
    auto i = find_if(
            mTriggers,
            [&fds](const Trigger &t) { return testCondition(t, fds); });
    if (i == mTriggers.end())
        return false;
    std::move(mPromise).setResult(std::move(*i));
    return true;
}

void PendingEvent::failWithCurrentException() {
    std::move(mPromise).failWithCurrentException();
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

FdSetTriple computeFdsToAwaitAndRemoveErroredEvents(
        const Api &api, std::multimap<TimePoint, PendingEvent> &events) {
    FdSetTriple fds;
    for (auto i = events.begin(); i != events.end(); ) {
        auto &event = i->second;
        try {
            event.addFdsToAwait(api, fds);
            ++i;
        } catch (...) {
            event.failWithCurrentException();
            i = events.erase(i);
        }
    }
    return fds;
}

void AwaiterImpl::awaitEvents() {
    while (!mPendingEvents.empty()) { // TODO refactor
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

        FdSetTriple fds =
                computeFdsToAwaitAndRemoveErroredEvents(mApi, mPendingEvents);

        if (mPendingEvents.empty())
            break;

        mApi.pselect(
                fds.bound,
                fds.readFds.get(),
                fds.writeFds.get(),
                fds.errorFds.get(),
                timeout,
                nullptr); //FIXME

        for (auto i = mPendingEvents.begin(); i != mPendingEvents.end(); ) {
            PendingEvent &e = i->second;
            if (e.testConditionAndFire(fds))
                i = mPendingEvents.erase(i);
            else
                ++i;
        }
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
