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

#ifndef INCLUDED_async_future_tcc
#define INCLUDED_async_future_tcc

#include "buildconfig.h"
#include "future.hh"

#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include "common/function_helper.hh"
#include "common/shared_function.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
template<typename Function>
typename std::enable_if<std::is_void<typename std::result_of<
        typename std::decay<Function>::type(common::trial<T> &&)
>::type>::value>::type
future_base<T>::then(Function &&f) && {
    future_base copy = std::move(*this);
    copy.delay().set_callback(std::forward<Function>(f));
}

template<typename T>
std::pair<promise<T>, future<T>> make_promise_future_pair() {
    auto d = std::make_shared<delay<T>>();
    return std::pair<promise<T>, future<T>>(
            std::piecewise_construct,
            std::forward_as_tuple(d),
            std::forward_as_tuple(d));
}

template<typename To, typename Function>
class composer {

private:

    Function m_function;
    promise<To> m_receiver;

public:

    template<typename F>
    composer(F &&function, promise<To> &&receiver) :
            m_function(std::forward<F>(function)),
            m_receiver(std::move(receiver)) { }

    template<typename From>
    void operator()(From &&r) {
        std::move(m_receiver).set_result_from([this, &r] {
            return common::invoke(m_function, std::forward<From>(r));
        });
    }

};

template<typename From>
template<typename Function, typename To>
void future_base<From>::then(Function &&f, promise<To> &&p) && {
    using C = composer<To, typename std::decay<Function>::type>;
    std::move(*this).then(common::shared_function<C>::create(
            std::forward<Function>(f), std::move(p)));
}

template<typename From>
template<typename Function, typename To>
typename std::enable_if<!std::is_void<To>::value, future<To>>::type
future_base<From>::then(Function &&f) && {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    std::move(*this).then(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename Function>
class mapper {

private:

    Function m_function;

public:

    template<typename F>
    mapper(F &&function) : m_function(std::forward<F>(function)) { }

    template<typename From>
    auto operator()(const common::trial<From> &r)
            -> typename common::result_of<Function &(const From &)>::type {
        return common::invoke(m_function, r.get());
    }

    template<typename From>
    auto operator()(common::trial<From> &&r)
            -> typename common::result_of<Function &(From &&)>::type {
        return common::invoke(m_function, std::move(r.get()));
    }

};

template<typename From>
template<typename Function, typename To>
void future_base<From>::map(Function &&f, promise<To> &&p) && {
    using M = mapper<typename std::decay<Function>::type>;
    std::move(*this).then(M(std::forward<Function>(f)), std::move(p));
}

template<typename From>
template<typename Function, typename To>
future<To> future_base<From>::map(Function &&f) && {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    std::move(*this).map(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename Function>
class recoverer {

private:

    Function m_function;

public:

    template<typename F>
    recoverer(F &&function) : m_function(std::forward<F>(function)) { }

    template<typename T>
    T operator()(const common::trial<T> &r) {
        if (r)
            return *r;
        return common::invoke(
                m_function, r.template value<std::exception_ptr>());
    }

    template<typename T>
    T operator()(common::trial<T> &&r) {
        if (r)
            return std::move(*r);
        return common::invoke(
                m_function, r.template value<std::exception_ptr>());
    }

};

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value>::type
future_base<T>::recover(F &&function, promise<T> &&p) && {
    using R = recoverer<typename std::decay<F>::type>;
    std::move(*this).then(R(std::forward<F>(function)), std::move(p));
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value, future<T>>::type
future_base<T>::recover(F &&function) && {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(*this).recover(std::forward<F>(function), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void future_base<T>::forward(promise<T> &&receiver) && {
    // std::move(*this).map(common::identity(), std::move(receiver));
    /*
     * Mapping would leave intermediate delay objects that will not be
     * deallocated until the final result is set. This would cause an
     * infinitely recursive algorithm to grow the delay object chain until it
     * eats up the heap.
     */
    delay_holder<T>::forward(std::move(*this), std::move(receiver));
}

template<typename F>
auto make_future_from(F &&f) -> future<typename std::result_of<F()>::type> {
    using T = typename std::result_of<F()>::type;
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(pf.first).set_result_from(std::forward<F>(f));
    return std::move(pf.second);
}

template<typename T, typename... Arg>
future<T> make_future(Arg &&... arg) {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(pf.first).set_result(std::forward<Arg>(arg)...);
    return std::move(pf.second);
}

template<typename T>
auto make_future_of(T &&t) -> future<typename std::decay<T>::type> {
    return make_future<typename std::decay<T>::type>(std::forward<T>(t));
}

template<typename T>
future<T> make_failed_future(std::exception_ptr e) {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(pf.first).fail(e);
    return std::move(pf.second);
}

template<typename T, typename E>
future<T> make_failed_future_of(E &&e) {
    return make_failed_future<T>(std::make_exception_ptr(std::forward<E>(e)));
}

template<typename T>
void future_base<T>::wrap(promise<future<T>> &&p) && {
    std::move(*this).map(make_future_of<T>, std::move(p));
}

template<typename T>
future<future<T>> future_base<T>::wrap() && {
    std::pair<promise<future<T>>, future<future<T>>> pf =
            make_promise_future_pair<future<T>>();
    std::move(*this).wrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
class unwrapper {

private:

    promise<T> m_receiver;

public:

    explicit unwrapper(promise<T> &&receiver) noexcept :
            m_receiver(std::move(receiver)) { }

    void operator()(common::trial<future<T>> &&r) {
        if (r)
            return std::move(*r).forward(std::move(m_receiver));

        std::move(m_receiver).fail(r.template value<std::exception_ptr>());
    }

};

template<typename T>
void future<future<T>>::unwrap(promise<T> &&p) && {
    std::move(*this).then(
            common::shared_function<unwrapper<T>>::create(std::move(p)));
}

template<typename T>
future<T> future<future<T>>::unwrap() && {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(*this).unwrap(std::move(pf.first));
    return std::move(pf.second);
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_future_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
