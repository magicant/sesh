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

#ifndef INCLUDED_os_signaling_handler_configuration_api_test_helper_hh
#define INCLUDED_os_signaling_handler_configuration_api_test_helper_hh

#include "buildconfig.h"

#include <cassert>
#include <map>
#include <memory>
#include <system_error>
#include "os/signaling/handler_configuration_api.hh"
#include "os/signaling/signal_number.hh"
#include "os/signaling/SignalNumberSetTestHelper.hh"

namespace sesh {
namespace os {
namespace signaling {

class handler_configuration_api_dummy : public handler_configuration_api {

    std::unique_ptr<SignalNumberSet> create_signal_number_set() const
            override {
        throw "unexpected create_signal_number_set";
    }

    std::error_code sigprocmask(
            mask_change_how,
            const SignalNumberSet *,
            SignalNumberSet *) const override {
        throw "unexpected sigprocmask";
    }

    std::error_code sigaction(
            signal_number,
            const signal_action *,
            signal_action *) const override {
        throw "unexpected sigaction";
    }

}; // class handler_configuration_api_dummy

class handler_configuration_api_fake : public handler_configuration_api {

public:

    constexpr static signal_number invalid_signal_number = 0;

private:

    mutable SignalNumberSetFake m_mask;
    mutable std::map<signal_number, signal_action> m_actions;

public:

    SignalNumberSetFake &signal_mask() noexcept { return m_mask; }
    const SignalNumberSetFake &signal_mask() const noexcept { return m_mask; }

    std::unique_ptr<SignalNumberSet> create_signal_number_set() const
            override {
        return std::unique_ptr<SignalNumberSet>(new SignalNumberSetFake);
    }

    std::error_code sigprocmask(
            mask_change_how how,
            const SignalNumberSet *new_mask,
            SignalNumberSet *old_mask) const override {
        const SignalNumberSetFake *fake_new_mask =
                dynamic_cast<const SignalNumberSetFake *>(new_mask);
        SignalNumberSetFake *fake_old_mask =
                dynamic_cast<SignalNumberSetFake *>(old_mask);

        assert((new_mask != nullptr) == (fake_new_mask != nullptr));
        assert((old_mask != nullptr) == (fake_old_mask != nullptr));

        if (fake_new_mask == fake_old_mask)
            return std::error_code();

        if (fake_old_mask != nullptr)
            *fake_old_mask = m_mask;

        if (fake_new_mask != nullptr) {
            switch (how) {
            case mask_change_how::block:
                m_mask.insertAll(*fake_new_mask);
                break;
            case mask_change_how::unblock:
                m_mask.eraseAll(*fake_new_mask);
                break;
            case mask_change_how::set_mask:
                m_mask = *fake_new_mask;
                break;
            }
        }

        return std::error_code();
    }

    std::map<signal_number, signal_action> &actions() noexcept {
        return m_actions;
    }
    const std::map<signal_number, signal_action> &actions() const noexcept {
        return m_actions;
    }

    std::error_code sigaction(
            signal_number n,
            const signal_action *new_action,
            signal_action *old_action) const override {
        if (n == invalid_signal_number)
            return std::make_error_code(std::errc::invalid_argument);

        signal_action &a =
                m_actions.emplace(n, default_action()).first->second;
        if (old_action != nullptr)
            *old_action = a;
        if (new_action != nullptr)
            a = *new_action;
        return std::error_code();
    }

}; // class handler_configuration_api_fake

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_handler_configuration_api_test_helper_h

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
