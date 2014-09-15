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

#ifndef INCLUDED_async_shared_future_tcc
#define INCLUDED_async_shared_future_tcc

#include "buildconfig.h"
#include "shared_future.hh"

#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include "async/future.tcc"
#include "common/either.hh"
#include "common/identity.hh"
#include "common/trial.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
class shared_future_base<T>::impl {

public:

    using callback = std::function<void(const common::trial<T> &)>;

private:

    common::maybe<common::trial<T>> m_result;

    /** Empty after the result is set and all callbacks are called. */
    std::vector<callback> m_callbacks;

public:

    void set_result(common::trial<T> &&);

    void add_callback(callback &&);

}; // template<typename T> class shared_future_base<T>::impl

template<typename T>
void shared_future_base<T>::impl::set_result(common::trial<T> &&t) {
    try {
        m_result.try_emplace(std::move(t));
    } catch (...) {
        m_result.try_emplace(std::current_exception());
    }

    for (callback &c : m_callbacks)
        c(*m_result);
    m_callbacks.clear();
}

template<typename T>
void shared_future_base<T>::impl::add_callback(callback &&c) {
    if (m_result)
        return c(*m_result);
    m_callbacks.push_back(std::move(c));
}

template<typename T>
shared_future_base<T>::shared_future_base(future<T> &&f) : m_impl() {
    if (!f.is_valid())
        return;

    m_impl = std::make_shared<impl>();

    auto &impl = m_impl;
    std::move(f).then([impl](common::trial<T> &&t) {
        impl->set_result(std::move(t));
    });
}

template<typename T>
bool shared_future_base<T>::operator==(const shared_future_base<T> &other)
        const noexcept {
    return this->m_impl == other.m_impl;
}

template<typename T>
bool shared_future_base<T>::operator!=(const shared_future_base<T> &other)
        const noexcept {
    return !(*this == other);
}

template<typename T>
bool shared_future_base<T>::is_valid() const noexcept {
    return m_impl != nullptr;
}

template<typename T>
shared_future_base<T>::operator bool() const noexcept {
    return is_valid();
}

template<typename T>
template<typename Function>
typename std::enable_if<std::is_void<typename std::result_of<
        typename std::decay<Function>::type(const common::trial<T> &)
>::type>::value>::type
shared_future_base<T>::then(Function &&f) const {
    m_impl->add_callback(std::forward<Function>(f));
}

template<typename From>
template<typename Function, typename To>
void shared_future_base<From>::then(Function &&f, promise<To> &&p) const {
    using C = composer<To, typename std::decay<Function>::type>;
    then(common::shared_function<C>::create(
            std::forward<Function>(f), std::move(p)));
}

template<typename From>
template<typename Function, typename To>
typename std::enable_if<!std::is_void<To>::value, future<To>>::type
shared_future_base<From>::then(Function &&f) const {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    then(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename From>
template<typename Function, typename To>
void shared_future_base<From>::map(Function &&f, promise<To> &&p) const {
    using M = mapper<typename std::decay<Function>::type>;
    then(M(std::forward<Function>(f)), std::move(p));
}

template<typename From>
template<typename Function, typename To>
future<To> shared_future_base<From>::map(Function &&f) const {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    map(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value>::type
shared_future_base<T>::recover(F &&function, promise<T> &&p) const {
    using R = recoverer<typename std::decay<F>::type>;
    then(R(std::forward<F>(function)), std::move(p));
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value, future<T>>::type
shared_future_base<T>::recover(F &&function) const {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    recover(std::forward<F>(function), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void shared_future_base<T>::forward(promise<T> &&receiver) const {
    map(common::identity(), std::move(receiver));
}

template<typename T>
void shared_future_base<T>::wrap(promise<future<T>> &&p) const {
    map(make_future_of<const T &>, std::move(p));
}

template<typename T>
future<future<T>> shared_future_base<T>::wrap() const {
    using F = future<T>;
    std::pair<promise<F>, future<F>> pf = make_promise_future_pair<F>();
    wrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void shared_future_base<T>::wrap_shared(promise<shared_future<T>> &&p) const {
    auto wrapper = [](const T &v) -> shared_future<T> {
        return make_future_of<const T &>(v);
    };
    map(wrapper, std::move(p));
}

template<typename T>
future<shared_future<T>> shared_future_base<T>::wrap_shared() const {
    using F = shared_future<T>;
    std::pair<promise<F>, future<F>> pf = make_promise_future_pair<F>();
    wrap_shared(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
class shared_unwrapper {

private:

    promise<T> m_receiver;

public:

    explicit shared_unwrapper(promise<T> &&receiver) noexcept :
            m_receiver(std::move(receiver)) { }

    void operator()(const common::trial<shared_future<T>> &r) {
        if (r)
            return r->forward(std::move(m_receiver));

        std::move(m_receiver).fail(r.template value<std::exception_ptr>());
    }

};

template<typename T>
void future<shared_future<T>>::unwrap(promise<T> &&p) && {
    using U = common::shared_function<shared_unwrapper<T>>;
    std::move(*this).then(U::create(std::move(p)));
}

template<typename T>
future<T> future<shared_future<T>>::unwrap() && {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(*this).unwrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void shared_future<shared_future<T>>::unwrap(promise<T> &&p) const {
    using U = common::shared_function<shared_unwrapper<T>>;
    this->then(U::create(std::move(p)));
}

template<typename T>
future<T> shared_future<shared_future<T>>::unwrap() const {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    unwrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
bool operator==(const shared_future_base<T> &f, std::nullptr_t) {
    return !f.is_valid();
}

template<typename T>
bool operator!=(const shared_future_base<T> &f, std::nullptr_t) {
    return f.is_valid();
}

template<typename T>
bool operator==(std::nullptr_t, const shared_future_base<T> &f) {
    return !f.is_valid();
}

template<typename T>
bool operator!=(std::nullptr_t, const shared_future_base<T> &f) {
    return f.is_valid();
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_shared_future_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
