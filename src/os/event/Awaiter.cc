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
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>
#include "async/Future.hh"
#include "async/promise.hh"
#include "common/container_helper.hh"
#include "common/shared_function.hh"
#include "common/trial.hh"
#include "common/variant.hh"
#include "helpermacros.h"
#include "os/TimeApi.hh"
#include "os/event/PselectApi.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/HandlerConfiguration.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::async::Future;
using sesh::async::promise;
using sesh::async::createPromiseFuturePair;
using sesh::common::find_if;
using sesh::common::shared_function;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::HandlerConfiguration;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using TimePoint = sesh::os::event::PselectApi::SteadyClockTime;
using Clock = TimePoint::clock;

namespace sesh {
namespace os {
namespace event {

namespace {

using FileDescriptorTrigger = variant<
        ReadableFileDescriptor, WritableFileDescriptor, ErrorFileDescriptor>;

class PendingEvent {

private:

    Timeout mTimeout;
    std::vector<FileDescriptorTrigger> mTriggers;
    promise<Trigger> mPromise;
    std::vector<HandlerConfiguration::Canceler> mCancelers;

public:

    explicit PendingEvent(promise<Trigger> p);
    PendingEvent(PendingEvent &&) = default;
    PendingEvent &operator=(PendingEvent &&) = default;
    ~PendingEvent();

    Timeout &timeout() noexcept { return mTimeout; }

    const std::vector<FileDescriptorTrigger> &triggers() const noexcept {
        return mTriggers;
    }

    void addTrigger(const FileDescriptorTrigger &t);

    bool hasFired() const noexcept { return !mPromise.is_valid(); }

    void fire(Trigger &&);
    void failWithCurrentException();

    void addCanceler(HandlerConfiguration::Canceler &&c);

};

class SignalHandler {

private:

    std::weak_ptr<PendingEvent> mEvent;

public:

    SignalHandler(const std::shared_ptr<PendingEvent> &) noexcept;

    void operator()(SignalNumber);

}; // class SignalHandler

class PselectArgument {

private:

    FileDescriptor::Value mFdBound;
    std::unique_ptr<FileDescriptorSet> mReadFds, mWriteFds, mErrorFds;
    TimePoint::duration mTimeout;

    void addFd(
            std::unique_ptr<FileDescriptorSet> &fds,
            FileDescriptor::Value fd,
            const PselectApi &api);

public:

    explicit PselectArgument(TimePoint::duration timeout) noexcept;

    /**
     * Updates this p-select argument according to the given trigger. May throw
     * some exception.
     */
    void add(const FileDescriptorTrigger &, const PselectApi &);

    /**
     * Updates this p-select argument according to the given event. This
     * function may fire the event directly if applicable.
     */
    void addOrFire(PendingEvent &, const PselectApi &);

    /** Calls the p-select API function with this argument. */
    std::error_code call(const PselectApi &api, const SignalNumberSet *);

    /** Tests if this p-select call result matches the given trigger. */
    bool matches(const FileDescriptorTrigger &) const;

    /**
     * Applies this p-select call result to the argument event. If the result
     * matches the event, the event is fired. If the event has already been
     * fired, this function has no side effect.
     */
    void applyResult(PendingEvent &) const;

}; // class PselectArgument

class AwaiterImpl : public Awaiter {

public:

    using TimeLimit = TimePoint;

private:

    const PselectApi &mApi;
    std::shared_ptr<HandlerConfiguration> mHandlerConfiguration;
    std::multimap<TimeLimit, std::shared_ptr<PendingEvent>> mPendingEvents;

    Future<Trigger> expectImpl(std::vector<Trigger> &&triggers) final override;

    bool removeFiredEvents();

    void fireTimeouts(TimePoint now);

    /** @return min for infinity */
    TimePoint::duration durationToNextTimeout(TimePoint now) const;

    PselectArgument computeArgumentFiringErroredEvents(TimePoint now);

    void applyResult(const PselectArgument &);

public:

    AwaiterImpl(
            const PselectApi &, std::shared_ptr<HandlerConfiguration> &&hc);

    void awaitEvents() final override;

}; // class AwaiterImpl

PendingEvent::PendingEvent(promise<Trigger> p) :
        mTimeout(Timeout::Interval::max()),
        mTriggers(),
        mPromise(std::move(p)),
        mCancelers() { }

PendingEvent::~PendingEvent() {
    for (HandlerConfiguration::Canceler &c : mCancelers)
        (void) c();
}

void PendingEvent::addTrigger(const FileDescriptorTrigger &t) {
    mTriggers.push_back(t);
}

void PendingEvent::fire(Trigger &&t) {
    if (!hasFired())
        std::move(mPromise).set_result(std::move(t));
}

void PendingEvent::failWithCurrentException() {
    if (!hasFired())
        std::move(mPromise).fail_with_current_exception();
}

void PendingEvent::addCanceler(HandlerConfiguration::Canceler &&c) {
    mCancelers.push_back(std::move(c));
}

SignalHandler::SignalHandler(const std::shared_ptr<PendingEvent> &e) noexcept :
        mEvent(e) { }

void SignalHandler::operator()(SignalNumber n) {
    if (std::shared_ptr<PendingEvent> e = mEvent.lock())
        e->fire(Signal(n));
}

PselectArgument::PselectArgument(TimePoint::duration timeout) noexcept :
        mFdBound(0),
        mReadFds(),
        mWriteFds(),
        mErrorFds(),
        mTimeout(timeout) { }

void PselectArgument::addFd(
        std::unique_ptr<FileDescriptorSet> &fds,
        FileDescriptor::Value fd,
        const PselectApi &api) {
    if (fds == nullptr)
        fds = api.createFileDescriptorSet();
    fds->set(fd);

    mFdBound = std::max(mFdBound, fd + 1);
}

void PselectArgument::add(
        const FileDescriptorTrigger &t, const PselectApi &api) {
    switch (t.tag()) {
    case FileDescriptorTrigger::tag<ReadableFileDescriptor>():
        addFd(mReadFds, t.value<ReadableFileDescriptor>().value(), api);
        return;
    case FileDescriptorTrigger::tag<WritableFileDescriptor>():
        addFd(mWriteFds, t.value<WritableFileDescriptor>().value(), api);
        return;
    case FileDescriptorTrigger::tag<ErrorFileDescriptor>():
        addFd(mErrorFds, t.value<ErrorFileDescriptor>().value(), api);
        return;
    }
    UNREACHABLE();
}

void PselectArgument::addOrFire(PendingEvent &e, const PselectApi &api) {
    if (e.hasFired())
        return;

    try {
        for (const FileDescriptorTrigger &t : e.triggers())
            add(t, api);
    } catch (...) {
        e.failWithCurrentException();
    }
}

std::error_code PselectArgument::call(
        const PselectApi &api, const SignalNumberSet *signalMask) {
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
    switch (t.tag()) {
    case FileDescriptorTrigger::tag<ReadableFileDescriptor>():
        return contains(mReadFds, t.value<ReadableFileDescriptor>().value());
    case FileDescriptorTrigger::tag<WritableFileDescriptor>():
        return contains(mWriteFds, t.value<WritableFileDescriptor>().value());
    case FileDescriptorTrigger::tag<ErrorFileDescriptor>():
        return contains(mErrorFds, t.value<ErrorFileDescriptor>().value());
    }
    UNREACHABLE();
}

void PselectArgument::applyResult(PendingEvent &e) const {
    if (e.hasFired())
        return;

    using namespace std::placeholders;
    auto i = find_if(
            e.triggers(), std::bind(&PselectArgument::matches, this, _1));
    if (i != e.triggers().end())
        e.fire(std::move(*i));
}

AwaiterImpl::AwaiterImpl(
        const PselectApi &api, std::shared_ptr<HandlerConfiguration> &&hc) :
        mApi(api), mHandlerConfiguration(std::move(hc)), mPendingEvents() {
    assert(mHandlerConfiguration != nullptr);
}

void registerSignalTrigger(
        Signal s, std::shared_ptr<PendingEvent> &e, HandlerConfiguration &hc) {
    auto result = hc.addHandler(
            s.number(), shared_function<SignalHandler>::create(e));
    switch (result.tag()) {
    case decltype(result)::tag<HandlerConfiguration::Canceler>():
        return e->addCanceler(
                std::move(result.value<HandlerConfiguration::Canceler>()));
    case decltype(result)::tag<std::error_code>():
        throw std::system_error(result.value<std::error_code>());
    }
}

void registerUserProvidedTrigger(
        UserProvidedTrigger &&t, std::shared_ptr<PendingEvent> &e) {
    using Result = UserProvidedTrigger::Result;
    std::weak_ptr<PendingEvent> w = e;
    std::move(t.future()).then([w](trial<Result> &&t) {
        if (std::shared_ptr<PendingEvent> e = w.lock()) {
            try {
                e->fire(UserProvidedTrigger(std::move(*t)));
            } catch (...) {
                e->failWithCurrentException();
            }
        }
    });
}

void registerTrigger(
        Trigger &&t,
        std::shared_ptr<PendingEvent> &e,
        HandlerConfiguration &hc) {
    switch (t.tag()) {
    case Trigger::tag<Timeout>():
        e->timeout() = std::min(e->timeout(), t.value<Timeout>());
        return;
    case Trigger::tag<ReadableFileDescriptor>():
        e->addTrigger(t.value<ReadableFileDescriptor>());
        return;
    case Trigger::tag<WritableFileDescriptor>():
        e->addTrigger(t.value<WritableFileDescriptor>());
        return;
    case Trigger::tag<ErrorFileDescriptor>():
        e->addTrigger(t.value<ErrorFileDescriptor>());
        return;
    case Trigger::tag<Signal>():
        registerSignalTrigger(t.value<Signal>(), e, hc);
        return;
    case Trigger::tag<UserProvidedTrigger>():
        registerUserProvidedTrigger(
                std::move(t.value<UserProvidedTrigger>()), e);
        return;
    }
}

TimePoint computeTimeLimit(Timeout timeout, const TimeApi &api) {
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

    auto event =
            std::make_shared<PendingEvent>(std::move(promiseAndFuture.first));
    for (Trigger &t : triggers)
        registerTrigger(std::move(t), event, *mHandlerConfiguration);

    TimePoint timeLimit = computeTimeLimit(event->timeout(), mApi);
    mPendingEvents.emplace(timeLimit, std::move(event));

    return std::move(promiseAndFuture.second);
}

bool AwaiterImpl::removeFiredEvents() {
    bool removedAny = false;
    for (auto i = mPendingEvents.begin(); i != mPendingEvents.end(); ) {
        PendingEvent &e = *i->second;
        if (e.hasFired()) {
            i = mPendingEvents.erase(i);
            removedAny = true;
        } else
            ++i;
    }
    return removedAny;
}

void AwaiterImpl::fireTimeouts(TimePoint now) {
    for (auto &p : mPendingEvents) {
        const TimeLimit &timeLimit = p.first;
        std::shared_ptr<PendingEvent> &e = p.second;
        if (timeLimit <= now)
            e->fire(e->timeout());
    }
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

PselectArgument AwaiterImpl::computeArgumentFiringErroredEvents(
        TimePoint now) {
    PselectArgument argument(durationToNextTimeout(now));
    for (auto &p : mPendingEvents)
        argument.addOrFire(*p.second, mApi);
    return argument;
}

void AwaiterImpl::applyResult(const PselectArgument &a) {
    for (auto &p : mPendingEvents)
        a.applyResult(*p.second);
}

void AwaiterImpl::awaitEvents() {
    while (!mPendingEvents.empty()) {
        TimePoint now = mApi.steadyClockNow();
        fireTimeouts(now);

        PselectArgument argument = computeArgumentFiringErroredEvents(now);
        if (removeFiredEvents())
            continue;

        std::error_code e = argument.call(
                mApi, mHandlerConfiguration->maskForPselect());
        assert(e != std::errc::bad_file_descriptor);

        mHandlerConfiguration->callHandlers();

        if (e)
            continue;
        applyResult(argument);
        removeFiredEvents();
    }
}

} // namespace

std::unique_ptr<Awaiter> createAwaiter(
        const PselectApi &api,
        std::shared_ptr<HandlerConfiguration> &&hc) {
    return std::unique_ptr<Awaiter>(new AwaiterImpl(api, std::move(hc)));
}

} // namespace event
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
