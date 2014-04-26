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

#ifndef INCLUDED_os_test_helper_SignalMaskApiFake_hh
#define INCLUDED_os_test_helper_SignalMaskApiFake_hh

#include "buildconfig.h"

#include <cassert>
#include <memory>
#include "os/signaling/SignalNumberSetTestHelper.hh"
#include "os/test_helper/UnimplementedApi.hh"

namespace sesh {
namespace os {
namespace test_helper {

class SignalMaskApiFake : public virtual UnimplementedApi {

private:

    mutable signaling::SignalNumberSetFake mMask;

public:

    signaling::SignalNumberSetFake &signalMask() noexcept {
        return mMask;
    }
    const signaling::SignalNumberSetFake &signalMask() const noexcept {
        return mMask;
    }

    std::unique_ptr<signaling::SignalNumberSet> createSignalNumberSet() const
            override {
        return std::unique_ptr<signaling::SignalNumberSet>(
                new signaling::SignalNumberSetFake);
    }

    std::error_code sigprocmask(
            MaskChangeHow how,
            const signaling::SignalNumberSet *newMask,
            signaling::SignalNumberSet *oldMask) const override {
        const signaling::SignalNumberSetFake *fakeNewMask =
                dynamic_cast<const signaling::SignalNumberSetFake *>(newMask);
        signaling::SignalNumberSetFake *fakeOldMask =
                dynamic_cast<signaling::SignalNumberSetFake *>(oldMask);

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

}; // class SignalMaskApiFake

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_SignalMaskApiFake_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
