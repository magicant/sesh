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

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include "common/DirectInitialize.hh"
#include "common/IntegerSequence.hh"
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

template<typename From, typename To, typename F, typename Function>
class Composer {

private:

    Function mFunction;
    Promise<To> mReceiver;

public:

    Composer(F &&function, Promise<To> &&receiver) :
            mFunction(std::forward<F>(function)),
            mReceiver(std::move(receiver)) { }

    void operator()(common::Try<From> &&r) {
        std::move(mReceiver).setResultFrom(
                [this, &r] { return mFunction(std::move(r)); });
    }

};

template<typename From>
template<typename F, typename Function, typename To>
Future<To> FutureBase<From>::then(F &&function) && {
    using C = Composer<From, To, F, Function>;
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    std::move(*this).setCallback(common::SharedFunction<C>::create(
            std::forward<F>(function), std::move(pf.first)));
    return std::move(pf.second);
}

template<typename From, typename To, typename F, typename Function>
class Mapper {

private:

    Function mFunction;
    Promise<To> mReceiver;

public:

    Mapper(F &&function, Promise<To> &&receiver) :
            mFunction(std::forward<F>(function)),
            mReceiver(std::move(receiver)) { }

    void operator()(common::Try<From> &&r) {
        std::move(mReceiver).setResultFrom(
                [this, &r] { return mFunction(std::move(*r)); });
    }

};

template<typename From>
template<typename F, typename Function, typename To>
Future<To> FutureBase<From>::map(F &&function) && {
    using C = Mapper<From, To, F, Function>;
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    std::move(*this).setCallback(common::SharedFunction<C>::create(
            std::forward<F>(function), std::move(pf.first)));
    return std::move(pf.second);
}

template<typename T, typename F, typename Function>
class Recoverer {

private:

    Function mFunction;
    Promise<T> mReceiver;

public:

    Recoverer(F &&function, Promise<T> &&receiver) :
            mFunction(std::forward<F>(function)),
            mReceiver(std::move(receiver)) { }

    void operator()(common::Try<T> &&r) {
        std::move(mReceiver).setResultFrom([this, &r]() -> T {
            if (r.hasValue())
                return std::move(*r);
            return mFunction(r.template value<std::exception_ptr>());
        });
    }

};

template<typename T>
template<typename F, typename Function>
Future<T> FutureBase<T>::recover(F &&function) && {
    using C = Recoverer<T, F, Function>;
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).setCallback(common::SharedFunction<C>::create(
            std::forward<F>(function), std::move(pf.first)));
    return std::move(pf.second);
}

template<typename T>
class Forwarder {

private:

    Promise<T> mReceiver;

public:

    explicit Forwarder(Promise<T> &&receiver) noexcept :
            mReceiver(std::move(receiver)) { }

    void operator()(common::Try<T> &&r) {
        std::move(mReceiver).setResultFrom([&r] { return std::move(*r); });
    }

};

template<typename T>
void FutureBase<T>::forward(Promise<T> &&receiver) && {
    std::move(*this).setCallback(
            common::SharedFunction<Forwarder<T>>::create(std::move(receiver)));
}

template<typename F>
auto createFutureFrom(F &&f) -> Future<typename std::result_of<F()>::type> {
    using T = typename std::result_of<F()>::type;
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(pf.first).setResultFrom(std::forward<F>(f));
    return std::move(pf.second);
}

template<typename T, typename... Arg>
class Constructor {

private:

    std::tuple<Arg &&...> mArguments;

    template<std::size_t... i>
    T construct(const common::IndexSequence<i...> &) {
        T t(std::forward<Arg>(std::get<i>(mArguments))...);
        return t;
    }

public:

    explicit Constructor(Arg &&... arg) :
            mArguments(std::forward<Arg>(arg)...) { }

    T operator()() {
        return construct(common::IndexSequenceFor<Arg...>());
    }

};

template<typename T, typename... Arg>
Future<T> createFuture(Arg &&... arg){
    /* XXX GCC 4.8.2 doesn't support capturing a parameter pack
    return createFutureFrom([&arg...]() -> T {
        T t(std::forward<Arg>(arg)...);
        return t;
    });
    */
    return createFutureFrom(Constructor<T, Arg...>(std::forward<Arg>(arg)...));
}

template<typename T>
auto createFutureOf(T &&t) -> Future<typename std::decay<T>::type> {
    return createFuture<typename std::decay<T>::type>(std::forward<T>(t));
}

template<typename T>
Future<T> createFailedFuture(std::exception_ptr e) {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(pf.first).fail(e);
    return std::move(pf.second);
}

template<typename T, typename E>
Future<T> createFailedFutureOf(E &&e) {
    return createFailedFuture<T>(std::make_exception_ptr(std::forward<E>(e)));
}

template<typename T>
class Unwrapper {

private:

    Promise<T> mReceiver;

public:

    explicit Unwrapper(Promise<T> &&receiver) noexcept :
            mReceiver(std::move(receiver)) { }

    void operator()(common::Try<Future<T>> &&r) {
        if (r.hasValue())
            return std::move(*r).forward(std::move(mReceiver));

        std::move(mReceiver).fail(r.template value<std::exception_ptr>());
    }

};

template<typename T>
Future<T> Future<Future<T>>::unwrap() && {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).setCallback(
            common::SharedFunction<Unwrapper<T>>::create(std::move(pf.first)));
    return std::move(pf.second);
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Future_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
