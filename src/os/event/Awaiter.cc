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

struct PendingEvent {
    std::vector<Trigger> triggers;
    Promise<Trigger> promise;
};

class PselectArgument {

private:

    FileDescriptor::Value mFdBound;
    std::unique_ptr<FileDescriptorSet> mReadFds, mWriteFds, mErrorFds;
    TimePoint::duration mTimeout;

    void addFd(
            std::unique_ptr<FileDescriptorSet> &fds,
            FileDescriptor::Value fd,
            const Api &api);

public:

    explicit PselectArgument(TimePoint::duration timeout) noexcept;

    /**
     * Updates this p-select argument according to the given trigger. May throw
     * some exception.
     */
    void add(const Trigger &, const Api &);

    /**
     * Updates this p-select argument according to the given event. This
     * function may fire the event directly if applicable.
     * @return true iff the event was fired.
     */
    bool addOrFire(PendingEvent &, const Api &);

    /** Calls the p-select API function with this argument. */
    std::error_code call(const Api &api);

    /** Tests if this p-select call result matches the given trigger. */
    bool matches(const Trigger &) const;

    /**
     * Applies this p-select call result to the argument event. If the result
     * matches the event, the event is fired.
     * @return true iff the event was fired.
     */
    bool applyResult(PendingEvent &) const;

}; // class PselectArgument

class AwaiterImpl : public Awaiter {

public:

    using TimeLimit = TimePoint;

private:

    const Api &mApi;
    std::multimap<TimeLimit, PendingEvent> mPendingEvents;

    Future<Trigger> expectImpl(std::vector<Trigger> &&triggers) final override;

    bool fireTimeouts(TimePoint now);

    /** @return min for infinity */
    TimePoint::duration durationToNextTimeout(TimePoint now) const;

    PselectArgument computeArgumentRemovingFailedEvents(TimePoint now);

    void applyResultRemovingDoneEvents(const PselectArgument &);

public:

    explicit AwaiterImpl(const Api &);

    void awaitEvents() final override;

}; // class AwaiterImpl

PselectArgument::PselectArgument(TimePoint::duration timeout) noexcept :
        mFdBound(0),
        mReadFds(),
        mWriteFds(),
        mErrorFds(),
        mTimeout(timeout) { }

void PselectArgument::addFd(
        std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd,
        const Api &api) {
    if (fds == nullptr)
        fds = api.createFileDescriptorSet();
    fds->set(fd);

    mFdBound = std::max(mFdBound, fd + 1);
}

void PselectArgument::add(const Trigger &t, const Api &api) {
    switch (t.index()) {
    case Trigger::index<Timeout>():
        return;
    case Trigger::index<ReadableFileDescriptor>():
        addFd(mReadFds, t.value<ReadableFileDescriptor>().value(), api);
        return;
    case Trigger::index<WritableFileDescriptor>():
        addFd(mWriteFds, t.value<WritableFileDescriptor>().value(), api);
        return;
    case Trigger::index<ErrorFileDescriptor>():
        addFd(mErrorFds, t.value<ErrorFileDescriptor>().value(), api);
        return;
    case Trigger::index<Signal>():
    case Trigger::index<UserProvidedTrigger>():
        throw "unexpected trigger type"; // TODO
    }
    UNREACHABLE();
}

bool PselectArgument::addOrFire(PendingEvent &e, const Api &api) {
    try {
        for (Trigger &t : e.triggers)
            add(t, api);
        return false;
    } catch (...) {
        std::move(e.promise).failWithCurrentException();
        return true;
    }
}

std::error_code PselectArgument::call(const Api &api) {
    return api.pselect(
            mFdBound,
            mReadFds.get(),
            mWriteFds.get(),
            mErrorFds.get(),
            mTimeout,
            nullptr); //TODO
}

bool contains(
        const std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd) {
    return fds != nullptr && fds->test(fd);
}

bool PselectArgument::matches(const Trigger &t) const {
    switch (t.index()) {
    case Trigger::index<Timeout>():
        return false;
    case Trigger::index<ReadableFileDescriptor>():
        return contains(mReadFds, t.value<ReadableFileDescriptor>().value());
    case Trigger::index<WritableFileDescriptor>():
        return contains(mWriteFds, t.value<WritableFileDescriptor>().value());
    case Trigger::index<ErrorFileDescriptor>():
        return contains(mErrorFds, t.value<ErrorFileDescriptor>().value());
    case Trigger::index<Signal>():
    case Trigger::index<UserProvidedTrigger>():
        return false; // TODO
    }
    UNREACHABLE();
}

bool PselectArgument::applyResult(PendingEvent &e) const {
    auto i = find_if(
            e.triggers, [this](const Trigger &t) { return matches(t); });
    if (i == e.triggers.end())
        return false;
    std::move(e.promise).setResult(std::move(*i));
    return true;
}

AwaiterImpl::AwaiterImpl(const Api &api) :
        mApi(api), mPendingEvents() { }

TimePoint computeTimeLimit(Timeout timeout, const Api &api) {
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

    TimePoint timeLimit = computeTimeLimit(minTimeout(triggers), mApi);
    mPendingEvents.emplace(
            timeLimit,
            PendingEvent{
                    std::move(triggers), std::move(promiseAndFuture.first)});
    return std::move(promiseAndFuture.second);
}

bool fireIfTimedOut(PendingEvent &e, TimePoint timeLimit, TimePoint now) {
    if (timeLimit > now)
        return false;
    std::move(e.promise).setResult(minTimeout(e.triggers));
    return true;
}

bool AwaiterImpl::fireTimeouts(TimePoint now) {
    bool fired = false;
    for (auto i = mPendingEvents.begin(); i != mPendingEvents.end(); ) {
        if (fireIfTimedOut(i->second, i->first, now)) {
            i = mPendingEvents.erase(i);
            fired = true;
        } else
            ++i;
    }
    return fired;
}

TimePoint::duration AwaiterImpl::durationToNextTimeout(TimePoint now) const {
    if (mPendingEvents.empty())
        return TimePoint::duration::min();

    TimePoint nextTimeLimit = mPendingEvents.begin()->first;
    if (nextTimeLimit == TimePoint::max())
        return TimePoint::duration::min();
    if (nextTimeLimit <= now)
        return TimePoint::duration::zero();
    return nextTimeLimit - now;
}

PselectArgument AwaiterImpl::computeArgumentRemovingFailedEvents(
        TimePoint now) {
    PselectArgument argument(durationToNextTimeout(now));
    for (auto i = mPendingEvents.begin(); i != mPendingEvents.end(); ) {
        if (argument.addOrFire(i->second, mApi))
            i = mPendingEvents.erase(i);
        else
            ++i;
    }
    return argument;
}

void AwaiterImpl::applyResultRemovingDoneEvents(const PselectArgument &a) {
    for (auto i = mPendingEvents.begin(); i != mPendingEvents.end(); ) {
        if (a.applyResult(i->second))
            i = mPendingEvents.erase(i);
        else
            ++i;
    }
}

void AwaiterImpl::awaitEvents() {
    while (!mPendingEvents.empty()) {
        TimePoint now = mApi.steadyClockNow();
        if (fireTimeouts(now))
            continue;

        PselectArgument argument = computeArgumentRemovingFailedEvents(now);

        if (mPendingEvents.empty())
            break;

        std::error_code e = argument.call(mApi);
        (void) e; // TODO

        applyResultRemovingDoneEvents(argument);
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
