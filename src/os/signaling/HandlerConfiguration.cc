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
#include "HandlerConfiguration.hh"

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <utility>
#include "common/Maybe.hh"
#include "common/SharedFunction.hh"
#include "helpermacros.h"
#include "os/signaling/HandlerConfigurationApi.hh"
#include "os/signaling/SignalErrorCode.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::Maybe;
using sesh::common::SharedFunction;
using sesh::common::createMaybeOf;
using sesh::common::makeSharedFunction;

namespace sesh {
namespace os {
namespace signaling {

namespace {

using AddHandlerResult = HandlerConfiguration::AddHandlerResult;
using Canceler = HandlerConfiguration::Canceler;
using Handler = HandlerConfiguration::Handler;
using MaskChangeHow = HandlerConfigurationApi::MaskChangeHow;
using SignalAction = HandlerConfigurationApi::SignalAction;
using TrapDefault = HandlerConfiguration::Default;

enum class ActionType {
    DEFAULT, IGNORE, HANDLER,
};

constexpr bool needsBlocking(ActionType a) noexcept {
    /* XXX C++14
    switch (a) {
    case ActionType::DEFAULT:
    case ActionType::IGNORE:
        return false;
    case ActionType::HANDLER:
        return true;
    }
    UNREACHABLE();
    */
    return a == ActionType::HANDLER;
}

/** Handler configuration for a single signal number. */
class SignalConfiguration {

public:

    using TrapAction = HandlerConfiguration::TrapAction;

private:

    std::list<Handler> mHandlers;
    TrapAction mTrapAction = TrapDefault();

public:

    /**
     * Adds a handler. The returned canceler function must not be called after
     * this signal configuration instance was destroyed. It must not be called
     * more than once.
     */
    std::function<void()> addHandler(Handler &&h) {
        mHandlers.push_front(std::move(h));
        auto i = mHandlers.begin();
        return [this, i]() { mHandlers.erase(i); };
    }

    void setTrap(TrapAction &&a) {
        mTrapAction = std::move(a);
    }

    void callHandlers(SignalNumber n) {
        for (const Handler &h : mHandlers)
            h(n);

        switch (mTrapAction.tag()) {
        case TrapAction::tag<TrapDefault>():
            break;
        case TrapAction::tag<Handler>():
            if (const auto &h = mTrapAction.value<Handler>())
                h(n);
            break;
        }
    }

    ActionType nativeActionType() const {
        if (!mHandlers.empty())
            return ActionType::HANDLER;

        switch (mTrapAction.tag()) {
        case TrapAction::tag<TrapDefault>():
            return ActionType::DEFAULT;
        case TrapAction::tag<Handler>():
            if (mTrapAction.value<Handler>() == nullptr)
                return ActionType::IGNORE;
            else
                return ActionType::HANDLER;
        }
        UNREACHABLE();
    }

}; // class SignalConfiguration

extern "C" void nativeCatchSignal(int);

SignalAction actionForType(ActionType type) {
    switch (type) {
    case ActionType::DEFAULT:
        return HandlerConfigurationApi::Default();
    case ActionType::IGNORE:
        return HandlerConfigurationApi::Ignore();
    case ActionType::HANDLER:
        return nativeCatchSignal;
    }
    UNREACHABLE();
}

class SignalData {

private:

    Maybe<SignalAction> mInitialAction;

public:

    SignalConfiguration configuration;

    /** Current action configuration on the native side. */
    Maybe<ActionType> nativeAction;

    /** Number of signal instances caught. */
    unsigned catchCount = 0;

    /**
     * Calls API's sigaction and remembers the old action if this is the first
     * call.
     */
    std::error_code sigaction(
            const HandlerConfigurationApi &api,
            SignalNumber n,
            const SignalAction *newAction) {
        SignalAction oldAction = HandlerConfigurationApi::Default();
        std::error_code e = api.sigaction(n, newAction, &oldAction);
        if (!mInitialAction.hasValue())
            mInitialAction.emplace(std::move(oldAction));
        return e;
    }

    std::error_code getInitialActionIfUnknown(
            const HandlerConfigurationApi &api, SignalNumber n) {
        if (mInitialAction.hasValue())
            return std::error_code();
        return sigaction(api, n, nullptr);
    }

    const Maybe<SignalAction> &initialAction() const noexcept {
        return mInitialAction;
    }

}; // class SignalData

/** Implementation of HandlerConfiguration. */
class HandlerConfigurationImpl :
        public HandlerConfiguration,
        public std::enable_shared_from_this<HandlerConfigurationImpl> {

private:

    const HandlerConfigurationApi &mApi;

    std::map<SignalNumber, SignalData> mData;

    /** Null until {@code #initializeMasks()} is called. */
    std::unique_ptr<SignalNumberSet> mInitialMask;
    /** Null until {@code #initializeMasks()} is called. */
    std::unique_ptr<SignalNumberSet> mMaskForPselect;

    /** Gets (or creates) the configuration for the argument signal number. */
    SignalConfiguration &configuration(SignalNumber n) {
        return mData[n].configuration;
    }

    std::error_code initializeMasks() {
        if (mMaskForPselect != nullptr)
            return std::error_code();

        mInitialMask = mApi.createSignalNumberSet();
        if (std::error_code e = mApi.sigprocmask(
                MaskChangeHow::BLOCK, nullptr, mInitialMask.get()))
            return e;

        mMaskForPselect = mInitialMask->clone();
        return std::error_code();
    }

    bool maskForPselect(SignalNumber n, ActionType a) {
        switch (a) {
        case ActionType::DEFAULT:
            return mInitialMask->test(n);
        case ActionType::IGNORE:
        case ActionType::HANDLER:
            return false;
        }
        UNREACHABLE();
    }

    std::error_code updateConfiguration(SignalNumber n, SignalData &data) {
        if (std::error_code e = initializeMasks())
            return e;

        ActionType newType = data.configuration.nativeActionType();
        Maybe<ActionType> maybeNewType = newType;
        if (maybeNewType == data.nativeAction)
            return std::error_code(); // no change, just return

        if (needsBlocking(newType))
            if (std::error_code e = mApi.sigprocmaskBlock(n))
                return e;

        SignalAction a = actionForType(newType);
        if (std::error_code e = data.sigaction(mApi, n, &a))
            return e;

        if (!needsBlocking(newType) && !mInitialMask->test(n))
            if (std::error_code e = mApi.sigprocmaskUnblock(n))
                return e;

        mMaskForPselect->set(n, maskForPselect(n, newType));
        data.nativeAction = maybeNewType;
        return std::error_code();
    }

    /** The function object returned as a handler canceler. */
    class HandlerCanceler {

    private:

        SignalNumber mNumber;
        std::shared_ptr<HandlerConfigurationImpl> mConfiguration;
        std::function<void()> mCanceler;
        bool mHasCanceled;

    public:

        HandlerCanceler(
                SignalNumber n,
                HandlerConfigurationImpl &c,
                std::function<void()> &&canceler) :
                mNumber(n),
                mConfiguration(c.shared_from_this()),
                mCanceler(std::move(canceler)),
                mHasCanceled(false) { }

        std::error_code operator()() {
            if (mHasCanceled)
                return std::error_code();

            mHasCanceled = true;
            mCanceler();

            return mConfiguration->updateConfiguration(
                    mNumber, mConfiguration->mData[mNumber]);
        }

    }; // class InternalCanceler

public:

    explicit HandlerConfigurationImpl(const HandlerConfigurationApi &api)
            noexcept :
            mApi(api) { }

    AddHandlerResult addHandler(SignalNumber n, Handler &&h) final override {
        SignalData &data = mData[n];
        auto canceler = data.configuration.addHandler(std::move(h));

        if (std::error_code e = updateConfiguration(n, data)) {
            canceler();
            return e;
        }
        return AddHandlerResult::create<Canceler>(
                SharedFunction<HandlerCanceler>::create(
                        n, *this, std::move(canceler)));
    }

    std::error_code setTrap(SignalNumber n, TrapAction &&a, SettingPolicy p)
            final override {
        SignalData &data = mData[n];

        switch (p) {
        case SettingPolicy::FAIL_IF_IGNORED:
            if (std::error_code e = data.getInitialActionIfUnknown(mApi, n))
                return e;
            if (data.initialAction()->tag() ==
                    SignalAction::tag<HandlerConfigurationApi::Ignore>())
                return SignalErrorCode::INITIALLY_IGNORED;
            // Fall through
        case SettingPolicy::FORCE:
            break;
        }

        data.configuration.setTrap(std::move(a));
        return updateConfiguration(n, data);
    }

    const SignalNumberSet *maskForPselect() const final override {
        return mMaskForPselect.get();
    }

    void callHandlers() final override {
        for (auto &pair : mData) {
            const SignalNumber &n = pair.first;
            SignalData &data = pair.second;

            while (data.catchCount > 0) {
                --data.catchCount;
                data.configuration.callHandlers(n);
            }
        }
    }

    void increaseCatchCount(SignalNumber n) {
        ++mData[n].catchCount;
    }

}; // class HandlerConfigurationImpl

std::weak_ptr<HandlerConfigurationImpl> instance;

void nativeCatchSignal(int signalNumber) {
    if (auto sharedInstance = instance.lock())
        sharedInstance->increaseCatchCount(signalNumber);
}

} // namespace

auto HandlerConfiguration::create(const HandlerConfigurationApi &api)
        -> std::shared_ptr<HandlerConfiguration> {
    auto sharedInstance = std::make_shared<HandlerConfigurationImpl>(api);
    instance = sharedInstance;
    return std::move(sharedInstance);
}

} // namespace signaling
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
