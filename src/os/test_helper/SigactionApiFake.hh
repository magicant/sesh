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

#ifndef INCLUDED_os_test_helper_SigactionApiFake_hh
#define INCLUDED_os_test_helper_SigactionApiFake_hh

#include "buildconfig.h"

#include <map>
#include <system_error>
#include "os/signaling/SignalNumber.hh"
#include "os/test_helper/UnimplementedApi.hh"

namespace sesh {
namespace os {
namespace test_helper {

class SigactionApiFake : public virtual UnimplementedApi {

public:

    using Action = Api::SignalAction;

    constexpr static signaling::SignalNumber INVALID_SIGNAL_NUMBER = 0;

private:

    mutable std::map<signaling::SignalNumber, Action> mActions;

public:

    std::map<signaling::SignalNumber, Action> &actions() noexcept {
        return mActions;
    }
    const std::map<signaling::SignalNumber, Action> &actions() const noexcept {
        return mActions;
    }

    std::error_code sigaction(
            signaling::SignalNumber n,
            const SignalAction *newAction,
            SignalAction *oldAction) const override {
        if (n == INVALID_SIGNAL_NUMBER)
            return std::make_error_code(std::errc::invalid_argument);

        auto d = [] { return Api::Default(); };
        Action &a = mActions.emplace(n, d).first->second;
        if (oldAction != nullptr)
            *oldAction = a;
        if (newAction != nullptr)
            a = *newAction;
        return std::error_code();
    }

}; // class SigactionApiFake

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_SigactionApiFake_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
