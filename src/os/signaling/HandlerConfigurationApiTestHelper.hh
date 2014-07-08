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

#ifndef INCLUDED_os_signaling_HandlerConfigurationApiTestHelper_hh
#define INCLUDED_os_signaling_HandlerConfigurationApiTestHelper_hh

#include "buildconfig.h"

#include <cassert>
#include <map>
#include <memory>
#include <system_error>
#include "os/signaling/HandlerConfigurationApi.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSetTestHelper.hh"

namespace sesh {
namespace os {
namespace signaling {

class HandlerConfigurationApiFake : public HandlerConfigurationApi {

public:

    using Action = SignalAction;

    constexpr static SignalNumber INVALID_SIGNAL_NUMBER = 0;

private:

    mutable SignalNumberSetFake mMask;
    mutable std::map<SignalNumber, Action> mActions;

public:

    SignalNumberSetFake &signalMask() noexcept { return mMask; }
    const SignalNumberSetFake &signalMask() const noexcept { return mMask; }

    std::unique_ptr<SignalNumberSet> createSignalNumberSet() const override {
        return std::unique_ptr<SignalNumberSet>(new SignalNumberSetFake);
    }

    std::error_code sigprocmask(
            MaskChangeHow how,
            const SignalNumberSet *newMask,
            SignalNumberSet *oldMask) const override {
        const SignalNumberSetFake *fakeNewMask =
                dynamic_cast<const SignalNumberSetFake *>(newMask);
        SignalNumberSetFake *fakeOldMask =
                dynamic_cast<SignalNumberSetFake *>(oldMask);

        assert((newMask != nullptr) == (fakeNewMask != nullptr));
        assert((oldMask != nullptr) == (fakeOldMask != nullptr));

        if (fakeNewMask == fakeOldMask)
            return std::error_code();

        if (fakeOldMask != nullptr)
            *fakeOldMask = mMask;

        if (fakeNewMask != nullptr) {
            switch (how) {
            case MaskChangeHow::BLOCK:
                mMask.insertAll(*fakeNewMask);
                break;
            case MaskChangeHow::UNBLOCK:
                mMask.eraseAll(*fakeNewMask);
                break;
            case MaskChangeHow::SET_MASK:
                mMask = *fakeNewMask;
                break;
            }
        }

        return std::error_code();
    }

    std::map<SignalNumber, Action> &actions() noexcept {
        return mActions;
    }
    const std::map<SignalNumber, Action> &actions() const noexcept {
        return mActions;
    }

    std::error_code sigaction(
            SignalNumber n,
            const SignalAction *newAction,
            SignalAction *oldAction) const override {
        if (n == INVALID_SIGNAL_NUMBER)
            return std::make_error_code(std::errc::invalid_argument);

        Action &a = mActions.emplace(n, Default()).first->second;
        if (oldAction != nullptr)
            *oldAction = a;
        if (newAction != nullptr)
            a = *newAction;
        return std::error_code();
    }

}; // class HandlerConfigurationApiFake

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_HandlerConfigurationApiTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
