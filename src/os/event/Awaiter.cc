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
#include "common/Variant.hh"
#include "helpermacros.h"
#include "os/Api.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfiguration.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createPromiseFuturePair;
using sesh::common::Variant;
using sesh::common::find_if;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfiguration;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::Api::SteadyClockTime;
using Clock = TimePoint::clock;

namespace sesh {
namespace os {
namespace event {

namespace {

using FileDescriptorTrigger = Variant<
        ReadableFileDescriptor, WritableFileDescriptor, ErrorFileDescriptor>;

struct PendingEvent {

    Timeout timeout;
    std::vector<FileDescriptorTrigger> triggers;
    Promise<Trigger> promise;

    explicit PendingEvent(Promise<Trigger> p) :
            timeout(Timeout::Interval::max()),
            triggers(),
            promise(std::move(p)) { }

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
    void add(const FileDescriptorTrigger &, const Api &);

    /**
     * Updates this p-select argument according to the given event. This
     * function may fire the event directly if applicable.
     * @return true iff the event was fired.
     */
    bool addOrFire(PendingEvent &, const Api &);

    /** Calls the p-select API function with this argument. */
    std::error_code call(const Api &api, const SignalNumberSet *);

    /** Tests if this p-select call result matches the given trigger. */
    bool matches(const FileDescriptorTrigger &) const;

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
    std::shared_ptr<HandlerConfiguration> mHandlerConfiguration;
    std::multimap<TimeLimit, PendingEvent> mPendingEvents;

    Future<Trigger> expectImpl(std::vector<Trigger> &&triggers) final override;

    bool fireTimeouts(TimePoint now);

    /** @return min for infinity */
    TimePoint::duration durationToNextTimeout(TimePoint now) const;

    PselectArgument computeArgumentRemovingFailedEvents(TimePoint now);

    void applyResultRemovingDoneEvents(const PselectArgument &);

public:

    AwaiterImpl(const Api &, std::shared_ptr<HandlerConfiguration> &&hc);

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

void PselectArgument::add(const FileDescriptorTrigger &t, const Api &api) {
    switch (t.index()) {
    case FileDescriptorTrigger::index<ReadableFileDescriptor>():
        addFd(mReadFds, t.value<ReadableFileDescriptor>().value(), api);
        return;
    case FileDescriptorTrigger::index<WritableFileDescriptor>():
        addFd(mWriteFds, t.value<WritableFileDescriptor>().value(), api);
        return;
    case FileDescriptorTrigger::index<ErrorFileDescriptor>():
        addFd(mErrorFds, t.value<ErrorFileDescriptor>().value(), api);
        return;
    }
    UNREACHABLE();
}

bool PselectArgument::addOrFire(PendingEvent &e, const Api &api) {
    try {
        for (FileDescriptorTrigger &t : e.triggers)
            add(t, api);
        return false;
    } catch (...) {
        std::move(e.promise).failWithCurrentException();
        return true;
    }
}

std::error_code PselectArgument::call(
        const Api &api, const SignalNumberSet *signalMask) {
    return api.pselect(
            mFdBound,
            mReadFds.get(),
            mWriteFds.get(),
            mErrorFds.get(),
            mTimeout,
            signalMask);
}

bool contains(
        const std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd) {
    return fds != nullptr && fds->test(fd);
}

bool PselectArgument::matches(const FileDescriptorTrigger &t) const {
    switch (t.index()) {
    case FileDescriptorTrigger::index<ReadableFileDescriptor>():
        return contains(mReadFds, t.value<ReadableFileDescriptor>().value());
    case FileDescriptorTrigger::index<WritableFileDescriptor>():
        return contains(mWriteFds, t.value<WritableFileDescriptor>().value());
    case FileDescriptorTrigger::index<ErrorFileDescriptor>():
        return contains(mErrorFds, t.value<ErrorFileDescriptor>().value());
    }
    UNREACHABLE();
}

bool PselectArgument::applyResult(PendingEvent &e) const {
    using namespace std::placeholders;
    auto i = find_if(
            e.triggers, std::bind(&PselectArgument::matches, this, _1));
    if (i == e.triggers.end())
        return false;
    std::move(e.promise).setResult(std::move(*i));
    return true;
}

AwaiterImpl::AwaiterImpl(
        const Api &api, std::shared_ptr<HandlerConfiguration> &&hc) :
        mApi(api), mHandlerConfiguration(std::move(hc)), mPendingEvents() {
    assert(mHandlerConfiguration != nullptr);
}

void registerTrigger(Trigger &&t, PendingEvent &e) {
    switch (t.index()) {
    case Trigger::index<Timeout>():
        e.timeout = std::min(e.timeout, t.value<Timeout>());
        return;
    case Trigger::index<ReadableFileDescriptor>():
        e.triggers.push_back(t.value<ReadableFileDescriptor>());
        return;
    case Trigger::index<WritableFileDescriptor>():
        e.triggers.push_back(t.value<WritableFileDescriptor>());
        return;
    case Trigger::index<ErrorFileDescriptor>():
        e.triggers.push_back(t.value<ErrorFileDescriptor>());
        return;
    case Trigger::index<Signal>():
        // TODO
        return;
    case Trigger::index<UserProvidedTrigger>():
        // TODO
        return;
    }
}

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

    PendingEvent event(std::move(promiseAndFuture.first));
    for (Trigger &t : triggers)
        registerTrigger(std::move(t), event);

    TimePoint timeLimit = computeTimeLimit(event.timeout, mApi);
    mPendingEvents.emplace(timeLimit, std::move(event));

    return std::move(promiseAndFuture.second);
}

bool fireIfTimedOut(PendingEvent &e, TimePoint timeLimit, TimePoint now) {
    if (timeLimit > now)
        return false;
    std::move(e.promise).setResult(e.timeout);
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

        std::error_code e = argument.call(
                mApi, mHandlerConfiguration->maskForPselect());
        (void) e; // TODO

        applyResultRemovingDoneEvents(argument);
    }
}

} // namespace

std::unique_ptr<Awaiter> createAwaiter(
        const Api &api,
        std::shared_ptr<HandlerConfiguration> &&hc) {
    return std::unique_ptr<Awaiter>(new AwaiterImpl(api, std::move(hc)));
}

} // namespace event
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
