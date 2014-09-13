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
#include "handler_configuration.hh"

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <utility>
#include "common/maybe.hh"
#include "common/shared_function.hh"
#include "helpermacros.h"
#include "os/signaling/handler_configuration_api.hh"
#include "os/signaling/SignalErrorCode.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::make_maybe_of;
using sesh::common::maybe;
using sesh::common::shared_function;

namespace sesh {
namespace os {
namespace signaling {

namespace {

using canceler_type = handler_configuration::canceler_type;
using handler_type = handler_configuration::handler_type;
using MaskChangeHow = HandlerConfigurationApi::MaskChangeHow;
using SignalAction = HandlerConfigurationApi::SignalAction;

enum class action_type {
    default_action, ignore, handler,
};

constexpr bool needs_blocking(action_type a) noexcept {
    /* XXX C++14
    switch (a) {
    case action_type::default_action:
    case action_type::ignore:
        return false;
    case action_type::handler:
        return true;
    }
    UNREACHABLE();
    */
    return a == action_type::handler;
}

/** Handler configuration for a single signal number. */
class signal_configuration {

public:

    using trap_action = handler_configuration::trap_action;

private:

    std::list<handler_type> m_handlers;
    trap_action m_trap_action = handler_configuration::default_action();

public:

    /**
     * Adds a handler. The returned canceler function must not be called after
     * this signal configuration instance was destroyed. It must not be called
     * more than once.
     */
    std::function<void()> add_handler(handler_type &&h) {
        m_handlers.push_front(std::move(h));
        auto i = m_handlers.begin();
        return [this, i]() { m_handlers.erase(i); };
    }

    void set_trap(trap_action &&a) {
        m_trap_action = std::move(a);
    }

    void call_handlers(SignalNumber n) {
        for (const handler_type &h : m_handlers)
            h(n);

        switch (m_trap_action.tag()) {
        case trap_action::tag<handler_configuration::default_action>():
            break;
        case trap_action::tag<handler_type>():
            if (const auto &h = m_trap_action.value<handler_type>())
                h(n);
            break;
        }
    }

    action_type native_action_type() const {
        if (!m_handlers.empty())
            return action_type::handler;

        switch (m_trap_action.tag()) {
        case trap_action::tag<handler_configuration::default_action>():
            return action_type::default_action;
        case trap_action::tag<handler_type>():
            if (m_trap_action.value<handler_type>() == nullptr)
                return action_type::ignore;
            else
                return action_type::handler;
        }
        UNREACHABLE();
    }

}; // class signal_configuration

extern "C" void native_catch_signal(int);

SignalAction action_for_type(action_type type) {
    switch (type) {
    case action_type::default_action:
        return HandlerConfigurationApi::Default();
    case action_type::ignore:
        return HandlerConfigurationApi::Ignore();
    case action_type::handler:
        return native_catch_signal;
    }
    UNREACHABLE();
}

class signal_data {

private:

    maybe<SignalAction> m_initial_action;

public:

    signal_configuration configuration;

    /** Current action configuration on the native side. */
    maybe<action_type> native_action;

    /** Number of signal instances caught. */
    unsigned catch_count = 0;

    /**
     * Calls API's sigaction and remembers the old action if this is the first
     * call.
     */
    std::error_code sigaction(
            const HandlerConfigurationApi &api,
            SignalNumber n,
            const SignalAction *new_action) {
        SignalAction old_action = HandlerConfigurationApi::Default();
        std::error_code e = api.sigaction(n, new_action, &old_action);
        if (!m_initial_action.has_value())
            m_initial_action.emplace(std::move(old_action));
        return e;
    }

    std::error_code get_initial_action_if_unknown(
            const HandlerConfigurationApi &api, SignalNumber n) {
        if (m_initial_action.has_value())
            return std::error_code();
        return sigaction(api, n, nullptr);
    }

    const maybe<SignalAction> &initial_action() const noexcept {
        return m_initial_action;
    }

}; // class signal_data

/** Implementation of handler configuration. */
class handler_configuration_impl :
        public handler_configuration,
        public std::enable_shared_from_this<handler_configuration_impl> {

private:

    const HandlerConfigurationApi &m_api;

    std::map<SignalNumber, signal_data> m_data;

    /** Null until {@code #initialize_masks()} is called. */
    std::unique_ptr<SignalNumberSet> m_initial_mask;
    /** Null until {@code #initialize_masks()} is called. */
    std::unique_ptr<SignalNumberSet> m_mask_for_pselect;

    /** Gets (or creates) the configuration for the argument signal number. */
    signal_configuration &configuration(SignalNumber n) {
        return m_data[n].configuration;
    }

    std::error_code initialize_masks() {
        if (m_mask_for_pselect != nullptr)
            return std::error_code();

        m_initial_mask = m_api.createSignalNumberSet();
        if (std::error_code e = m_api.sigprocmask(
                MaskChangeHow::BLOCK, nullptr, m_initial_mask.get()))
            return e;

        m_mask_for_pselect = m_initial_mask->clone();
        return std::error_code();
    }

    bool mask_for_pselect(SignalNumber n, action_type a) {
        switch (a) {
        case action_type::default_action:
            return m_initial_mask->test(n);
        case action_type::ignore:
        case action_type::handler:
            return false;
        }
        UNREACHABLE();
    }

    std::error_code update_configuration(SignalNumber n, signal_data &data) {
        if (std::error_code e = initialize_masks())
            return e;

        action_type new_type = data.configuration.native_action_type();
        maybe<action_type> maybe_new_type = new_type;
        if (maybe_new_type == data.native_action)
            return std::error_code(); // no change, just return

        if (needs_blocking(new_type))
            if (std::error_code e = m_api.sigprocmaskBlock(n))
                return e;

        SignalAction a = action_for_type(new_type);
        if (std::error_code e = data.sigaction(m_api, n, &a))
            return e;

        if (!needs_blocking(new_type) && !m_initial_mask->test(n))
            if (std::error_code e = m_api.sigprocmaskUnblock(n))
                return e;

        m_mask_for_pselect->set(n, mask_for_pselect(n, new_type));
        data.native_action = maybe_new_type;
        return std::error_code();
    }

    /** The function object returned as a handler canceler. */
    class handler_canceler {

    private:

        SignalNumber m_number;
        std::shared_ptr<handler_configuration_impl> m_configuration;
        std::function<void()> m_canceler;
        bool m_has_canceled;

    public:

        handler_canceler(
                SignalNumber n,
                handler_configuration_impl &c,
                std::function<void()> &&canceler) :
                m_number(n),
                m_configuration(c.shared_from_this()),
                m_canceler(std::move(canceler)),
                m_has_canceled(false) { }

        std::error_code operator()() {
            if (m_has_canceled)
                return std::error_code();

            m_has_canceled = true;
            m_canceler();

            return m_configuration->update_configuration(
                    m_number, m_configuration->m_data[m_number]);
        }

    }; // class handler_canceler

public:

    explicit handler_configuration_impl(const HandlerConfigurationApi &api)
            noexcept :
            m_api(api) { }

    add_handler_result add_handler(SignalNumber n, handler_type &&h)
            final override {
        signal_data &data = m_data[n];
        auto canceler = data.configuration.add_handler(std::move(h));

        if (std::error_code e = update_configuration(n, data)) {
            canceler();
            return e;
        }
        return add_handler_result::create<canceler_type>(
                shared_function<handler_canceler>::create(
                        n, *this, std::move(canceler)));
    }

    std::error_code set_trap(SignalNumber n, trap_action &&a, setting_policy p)
            final override {
        signal_data &data = m_data[n];

        switch (p) {
        case setting_policy::fail_if_ignored:
            if (auto e = data.get_initial_action_if_unknown(m_api, n))
                return e;
            if (data.initial_action()->tag() ==
                    SignalAction::tag<HandlerConfigurationApi::Ignore>())
                return SignalErrorCode::INITIALLY_IGNORED;
            // Fall through
        case setting_policy::force:
            break;
        }

        data.configuration.set_trap(std::move(a));
        return update_configuration(n, data);
    }

    const SignalNumberSet *mask_for_pselect() const final override {
        return m_mask_for_pselect.get();
    }

    void call_handlers() final override {
        for (auto &pair : m_data) {
            const SignalNumber &n = pair.first;
            signal_data &data = pair.second;

            while (data.catch_count > 0) {
                --data.catch_count;
                data.configuration.call_handlers(n);
            }
        }
    }

    void increase_catch_count(SignalNumber n) {
        ++m_data[n].catch_count;
    }

}; // class handler_configuration_impl

std::weak_ptr<handler_configuration_impl> instance;

void native_catch_signal(int signal_number) {
    if (auto shared_instance = instance.lock())
        shared_instance->increase_catch_count(signal_number);
}

} // namespace

auto handler_configuration::create(const HandlerConfigurationApi &api)
        -> std::shared_ptr<handler_configuration> {
    auto shared_instance = std::make_shared<handler_configuration_impl>(api);
    instance = shared_instance;
    return std::move(shared_instance);
}

} // namespace signaling
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
