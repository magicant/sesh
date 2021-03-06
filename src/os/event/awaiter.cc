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
#include "awaiter.hh"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <memory>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "async/promise.hh"
#include "common/container_helper.hh"
#include "common/either.hh"
#include "common/shared_function.hh"
#include "common/variant.hh"
#include "helpermacros.h"
#include "os/event/pselect_api.hh"
#include "os/event/trigger.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/signaling/handler_configuration.hh"
#include "os/signaling/signal_number.hh"
#include "os/signaling/signal_number_set.hh"
#include "os/time_api.hh"

using sesh::async::future;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::find_if;
using sesh::common::shared_function;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::io::file_descriptor;
using sesh::os::io::file_descriptor_set;
using sesh::os::signaling::handler_configuration;
using sesh::os::signaling::signal_number;
using sesh::os::signaling::signal_number_set;

using time_point = sesh::os::event::pselect_api::steady_clock_time;

namespace sesh {
namespace os {
namespace event {

namespace {

using file_descriptor_trigger = variant<
        readable_file_descriptor,
        writable_file_descriptor,
        error_file_descriptor>;

class pending_event {

private:

    class timeout m_timeout;
    std::vector<file_descriptor_trigger> m_triggers;
    promise<trigger> m_promise;
    std::vector<handler_configuration::canceler_type> m_cancelers;

public:

    explicit pending_event(promise<trigger>);
    pending_event(pending_event &&) = default;
    pending_event &operator=(pending_event &&) = default;
    ~pending_event();

    class timeout &timeout() noexcept { return m_timeout; }

    const std::vector<file_descriptor_trigger> &triggers() const noexcept {
        return m_triggers;
    }

    void add_trigger(const file_descriptor_trigger &t);

    bool has_fired() const noexcept { return !m_promise.is_valid(); }

    void fire(trigger &&);
    void fail_with_current_exception();

    void add_canceler(handler_configuration::canceler_type &&c);

};

class signal_handler {

private:

    std::weak_ptr<pending_event> m_event;

public:

    signal_handler(const std::shared_ptr<pending_event> &) noexcept;

    void operator()(signal_number);

}; // class signal_handler

class pselect_argument {

private:

    file_descriptor::value_type m_fd_bound;
    std::unique_ptr<file_descriptor_set> m_read_fds, m_write_fds, m_error_fds;
    time_point::duration m_timeout;

    void add_fd(
            std::unique_ptr<file_descriptor_set> &fds,
            file_descriptor::value_type fd,
            const pselect_api &api);

public:

    explicit pselect_argument(time_point::duration timeout) noexcept;

    /**
     * Updates this p-select argument according to the given trigger. May throw
     * some exception.
     */
    void add(const file_descriptor_trigger &, const pselect_api &);

    /**
     * Updates this p-select argument according to the given event. This
     * function may fire the event directly if applicable.
     */
    void add_or_fire(pending_event &, const pselect_api &);

    /** Calls the p-select API function with this argument. */
    std::error_code call(const pselect_api &api, const signal_number_set *);

    /** Tests if this p-select call result matches the given trigger. */
    bool matches(const file_descriptor_trigger &) const;

    /**
     * Applies this p-select call result to the argument event. If the result
     * matches the event, the event is fired. If the event has already been
     * fired, this function has no side effect.
     */
    void apply_result(pending_event &) const;

}; // class pselect_argument

class awaiter_impl : public awaiter {

public:

    using time_limit = time_point;

private:

    const pselect_api &m_api;
    std::shared_ptr<handler_configuration> m_handler_configuration;
    std::multimap<time_limit, std::shared_ptr<pending_event>> m_pending_events;

    future<trigger> expect_impl(std::vector<trigger> &&triggers)
            final override;

    bool remove_fired_events();

    void fire_timeouts(time_point now);

    /** @return min for infinity */
    time_point::duration duration_to_next_timeout(time_point now) const;

    pselect_argument compute_argument_firing_errored_events(time_point now);

    void apply_result(const pselect_argument &);

public:

    awaiter_impl(
            const pselect_api &, std::shared_ptr<handler_configuration> &&hc);

    void await_events() final override;

}; // class awaiter_impl

pending_event::pending_event(promise<trigger> p) :
        m_timeout(timeout::internal_type::max()),
        m_triggers(),
        m_promise(std::move(p)),
        m_cancelers() { }

pending_event::~pending_event() {
    for (handler_configuration::canceler_type &c : m_cancelers)
        (void) c();
}

void pending_event::add_trigger(const file_descriptor_trigger &t) {
    m_triggers.push_back(t);
}

void pending_event::fire(trigger &&t) {
    if (!has_fired())
        std::move(m_promise).set_result(std::move(t));
}

void pending_event::fail_with_current_exception() {
    if (!has_fired())
        std::move(m_promise).fail_with_current_exception();
}

void pending_event::add_canceler(handler_configuration::canceler_type &&c) {
    m_cancelers.push_back(std::move(c));
}

signal_handler::signal_handler(const std::shared_ptr<pending_event> &e)
        noexcept :
        m_event(e) { }

void signal_handler::operator()(signal_number n) {
    if (std::shared_ptr<pending_event> e = m_event.lock())
        e->fire(signal(n));
}

pselect_argument::pselect_argument(time_point::duration timeout) noexcept :
        m_fd_bound(0),
        m_read_fds(),
        m_write_fds(),
        m_error_fds(),
        m_timeout(timeout) { }

void pselect_argument::add_fd(
        std::unique_ptr<file_descriptor_set> &fds,
        file_descriptor::value_type fd,
        const pselect_api &api) {
    if (fds == nullptr)
        fds = api.create_file_descriptor_set();
    fds->set(fd);

    m_fd_bound = std::max(m_fd_bound, fd + 1);
}

void pselect_argument::add(
        const file_descriptor_trigger &t, const pselect_api &api) {
    switch (t.tag()) {
    case file_descriptor_trigger::tag<readable_file_descriptor>():
        add_fd(m_read_fds, t.value<readable_file_descriptor>().value(), api);
        return;
    case file_descriptor_trigger::tag<writable_file_descriptor>():
        add_fd(m_write_fds, t.value<writable_file_descriptor>().value(), api);
        return;
    case file_descriptor_trigger::tag<error_file_descriptor>():
        add_fd(m_error_fds, t.value<error_file_descriptor>().value(), api);
        return;
    }
    UNREACHABLE();
}

void pselect_argument::add_or_fire(pending_event &e, const pselect_api &api) {
    if (e.has_fired())
        return;

    try {
        for (const file_descriptor_trigger &t : e.triggers())
            add(t, api);
    } catch (...) {
        e.fail_with_current_exception();
    }
}

std::error_code pselect_argument::call(
        const pselect_api &api, const signal_number_set *signal_mask) {
    return api.pselect(
            m_fd_bound,
            m_read_fds.get(),
            m_write_fds.get(),
            m_error_fds.get(),
            m_timeout,
            signal_mask);
}

bool contains(
        const std::unique_ptr<file_descriptor_set> &fds,
        file_descriptor::value_type fd) {
    return fds != nullptr && fds->test(fd);
}

bool pselect_argument::matches(const file_descriptor_trigger &t) const {
    switch (t.tag()) {
    case file_descriptor_trigger::tag<readable_file_descriptor>():
        return contains(
                m_read_fds, t.value<readable_file_descriptor>().value());
    case file_descriptor_trigger::tag<writable_file_descriptor>():
        return contains(
                m_write_fds, t.value<writable_file_descriptor>().value());
    case file_descriptor_trigger::tag<error_file_descriptor>():
        return contains(m_error_fds, t.value<error_file_descriptor>().value());
    }
    UNREACHABLE();
}

void pselect_argument::apply_result(pending_event &e) const {
    if (e.has_fired())
        return;

    using namespace std::placeholders;
    auto i = find_if(
            e.triggers(), std::bind(&pselect_argument::matches, this, _1));
    if (i != e.triggers().end())
        e.fire(std::move(*i));
}

awaiter_impl::awaiter_impl(
        const pselect_api &api, std::shared_ptr<handler_configuration> &&hc) :
        m_api(api),
        m_handler_configuration(std::move(hc)),
        m_pending_events() {
    assert(m_handler_configuration != nullptr);
}

void register_signal_trigger(
        signal s,
        std::shared_ptr<pending_event> &e,
        handler_configuration &hc) {
    auto result = hc.add_handler(
            s.number(), shared_function<signal_handler>::create(e));
    switch (result.tag()) {
    case decltype(result)::tag<handler_configuration::canceler_type>():
        return e->add_canceler(std::move(
                result.value<handler_configuration::canceler_type>()));
    case decltype(result)::tag<std::error_code>():
        throw std::system_error(result.value<std::error_code>());
    }
}

void register_user_provided_trigger(
        user_provided_trigger &&t, std::shared_ptr<pending_event> &e) {
    using result = user_provided_trigger::result_type;
    std::weak_ptr<pending_event> w = e;
    std::move(t.future()).then([w](trial<result> &&t) {
        if (std::shared_ptr<pending_event> e = w.lock()) {
            try {
                e->fire(user_provided_trigger(std::move(t.get())));
            } catch (...) {
                e->fail_with_current_exception();
            }
        }
    });
}

void register_trigger(
        trigger &&t,
        std::shared_ptr<pending_event> &e,
        handler_configuration &hc) {
    switch (t.tag()) {
    case trigger::tag<timeout>():
        e->timeout() = std::min(e->timeout(), t.value<timeout>());
        return;
    case trigger::tag<readable_file_descriptor>():
        e->add_trigger(t.value<readable_file_descriptor>());
        return;
    case trigger::tag<writable_file_descriptor>():
        e->add_trigger(t.value<writable_file_descriptor>());
        return;
    case trigger::tag<error_file_descriptor>():
        e->add_trigger(t.value<error_file_descriptor>());
        return;
    case trigger::tag<signal>():
        register_signal_trigger(t.value<signal>(), e, hc);
        return;
    case trigger::tag<user_provided_trigger>():
        register_user_provided_trigger(
                std::move(t.value<user_provided_trigger>()), e);
        return;
    }
}

time_point compute_time_limit(timeout to, const time_api &api) {
    if (to.interval() < timeout::internal_type::zero())
        to = timeout(timeout::internal_type::zero());

    if (to.interval() == timeout::internal_type::max())
        return time_point::max();

    time_point now = api.steady_clock_now();
    if (now > time_point::max() - to.interval())
        return time_point::max();
    return now + to.interval();
}

future<trigger> awaiter_impl::expect_impl(
        std::vector<trigger> &&triggers) {
    auto pf = make_promise_future_pair<trigger>();
    if (triggers.empty())
        return std::move(pf.second);

    auto event = std::make_shared<pending_event>(std::move(pf.first));
    for (trigger &t : triggers)
        register_trigger(std::move(t), event, *m_handler_configuration);

    time_point time_limit = compute_time_limit(event->timeout(), m_api);
    m_pending_events.emplace(time_limit, std::move(event));

    return std::move(pf.second);
}

bool awaiter_impl::remove_fired_events() {
    bool removed_any = false;
    for (auto i = m_pending_events.begin(); i != m_pending_events.end(); ) {
        pending_event &e = *i->second;
        if (e.has_fired()) {
            i = m_pending_events.erase(i);
            removed_any = true;
        } else
            ++i;
    }
    return removed_any;
}

void awaiter_impl::fire_timeouts(time_point now) {
    for (auto &p : m_pending_events) {
        const time_limit &limit = p.first;
        std::shared_ptr<pending_event> &e = p.second;
        if (limit <= now)
            e->fire(e->timeout());
    }
}

time_point::duration awaiter_impl::duration_to_next_timeout(time_point now)
        const {
    if (m_pending_events.empty())
        return time_point::duration::min();

    time_point next_time_limit = m_pending_events.begin()->first;
    if (next_time_limit == time_point::max())
        return time_point::duration::min();
    if (next_time_limit <= now)
        return time_point::duration::zero();
    return next_time_limit - now;
}

pselect_argument awaiter_impl::compute_argument_firing_errored_events(
        time_point now) {
    pselect_argument argument(duration_to_next_timeout(now));
    for (auto &p : m_pending_events)
        argument.add_or_fire(*p.second, m_api);
    return argument;
}

void awaiter_impl::apply_result(const pselect_argument &a) {
    for (auto &p : m_pending_events)
        a.apply_result(*p.second);
}

void awaiter_impl::await_events() {
    while (!m_pending_events.empty()) {
        time_point now = m_api.steady_clock_now();
        fire_timeouts(now);

        pselect_argument argument =
                compute_argument_firing_errored_events(now);
        if (remove_fired_events())
            continue;

        std::error_code e = argument.call(
                m_api, m_handler_configuration->mask_for_pselect());
        assert(e != std::errc::bad_file_descriptor);

        m_handler_configuration->call_handlers();

        if (e)
            continue;
        apply_result(argument);
        remove_fired_events();
    }
}

} // namespace

std::unique_ptr<awaiter> create_awaiter(
        const pselect_api &api,
        std::shared_ptr<handler_configuration> &&hc) {
    return std::unique_ptr<awaiter>(new awaiter_impl(api, std::move(hc)));
}

} // namespace event
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
