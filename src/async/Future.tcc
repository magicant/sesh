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

#ifndef INCLUDED_async_Future_tcc
#define INCLUDED_async_Future_tcc

#include "buildconfig.h"
#include "Future.hh"

#include <functional>
#include <utility>
#include "common/DirectInitialize.hh"
#include "common/SharedFunction.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
void FutureBase<T>::setCallback(Callback &&f) && {
    this->delay().setCallback(std::move(f));
    this->invalidate();
}

template<typename T>
std::pair<Promise<T>, Future<T>> createPromiseFuturePair() {
    auto delay = std::make_shared<Delay<T>>();
    return std::pair<Promise<T>, Future<T>>(
            std::piecewise_construct,
            std::forward_as_tuple(delay),
            std::forward_as_tuple(delay));
}

template<typename From>
template<typename F, typename Function, typename To>
Future<To> FutureBase<From>::then(F &&function) && {
    class Composer {
    private:
        Function mFunction;
        Promise<To> mReceiver;
    public:
        Composer(F &&function, Promise<To> &&receiver) :
                mFunction(std::forward<F>(function)),
                mReceiver(std::move(receiver)) { }
        void operator()(Result<From> &&r) {
            std::move(mReceiver).setResultFrom(
                    [this, &r] { return mFunction(std::move(*r)); });
        }
    };

    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    std::move(*this).setCallback(common::SharedFunction<Composer>(
            common::DIRECT_INITIALIZE,
            std::forward<F>(function),
            std::move(pf.first)));
    return std::move(pf.second);
}

template<typename T>
template<typename F, typename Function>
Future<T> FutureBase<T>::recover(F &&function) && {
    class Recoverer {
    private:
        Function mFunction;
        Promise<T> mReceiver;
    public:
        Recoverer(F &&function, Promise<T> &&receiver) :
                mFunction(std::forward<F>(function)),
                mReceiver(std::move(receiver)) { }
        void operator()(Result<T> &&r) {
            std::move(mReceiver).setResultFrom([this, &r]() -> T {
                if (r.hasValue())
                    return std::move(*r);
                return mFunction(r.template value<std::exception_ptr>());
            });
        }
    };

    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).setCallback(common::SharedFunction<Recoverer>(
            common::DIRECT_INITIALIZE,
            std::forward<F>(function),
            std::move(pf.first)));
    return std::move(pf.second);
}

template<typename T>
void FutureBase<T>::forward(Promise<T> &&receiver) && {
    class Forwarder {
    private:
        Promise<T> mReceiver;
    public:
        explicit Forwarder(Promise<T> &&receiver) noexcept :
                mReceiver(std::move(receiver)) { }
        void operator()(Result<T> &&r) {
            std::move(mReceiver).setResultFrom([&r] { return *r; });
        }
    };

    std::move(*this).setCallback(common::SharedFunction<Forwarder>(
            common::DIRECT_INITIALIZE, std::move(receiver)));
}

template<typename F>
auto createFutureFrom(F &&f) -> Future<typename std::result_of<F()>::type> {
    using T = typename std::result_of<F()>::type;
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(pf.first).setResultFrom(std::forward<F>(f));
    return std::move(pf.second);
}

template<typename T, typename... Arg>
Future<T> createFuture(Arg &&... arg){
    return createFutureFrom([&arg...]() -> T {
        T t(std::forward<Arg>(arg)...);
        return t;
    });
}

template<typename T>
auto createFutureOf(T &&t) -> Future<typename std::decay<T>::type> {
    return createFuture<typename std::decay<T>::type>(std::forward<T>(t));
}

template<typename T>
Future<T> createFailedFuture(std::exception_ptr e) {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(pf.first).setResultFrom(
            [e]() -> T { std::rethrow_exception(e); });
    return std::move(pf.second);
}

template<typename T, typename E>
Future<T> createFailedFutureOf(E &&e) {
    return createFailedFuture<T>(std::make_exception_ptr(std::forward<E>(e)));
}

template<typename T>
Future<T> Future<Future<T>>::unwrap() && {
    class Unwrapper {
    private:
        Promise<T> mReceiver;
    public:
        explicit Unwrapper(Promise<T> &&receiver) noexcept :
                mReceiver(std::move(receiver)) { }
        void operator()(Result<Future<T>> &&r) {
            if (r.hasValue())
                return std::move(*r).forward(std::move(mReceiver));

            std::exception_ptr &e = r.template value<std::exception_ptr>();
            std::move(mReceiver).setResultFrom([e]() -> T {
                std::rethrow_exception(e);
            });
        }
    };

    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).setCallback(common::SharedFunction<Unwrapper>(
            common::DIRECT_INITIALIZE, std::move(pf.first)));
    return std::move(pf.second);
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Future_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
