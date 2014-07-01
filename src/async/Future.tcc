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
#include "common/Identity.hh"
#include "common/IntegerSequence.hh"
#include "common/SharedFunction.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
template<typename Function>
typename std::enable_if<std::is_void<
        typename std::result_of<
                typename std::decay<Function>::type(common::Try<T> &&)>::type
>::value>::type
FutureBase<T>::then(Function &&f) && {
    this->delay().setCallback(std::forward<Function>(f));
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

template<typename To, typename Function>
class Composer {

private:

    Function mFunction;
    Promise<To> mReceiver;

public:

    template<typename F>
    Composer(F &&function, Promise<To> &&receiver) :
            mFunction(std::forward<F>(function)),
            mReceiver(std::move(receiver)) { }

    template<typename From>
    void operator()(From &&r) {
        std::move(mReceiver).setResultFrom(
                [this, &r] { return mFunction(std::forward<From>(r)); });
    }

};

template<typename From>
template<typename Function, typename To>
void FutureBase<From>::then(Function &&f, Promise<To> &&p) && {
    using C = Composer<To, typename std::decay<Function>::type>;
    std::move(*this).then(common::SharedFunction<C>::create(
            std::forward<Function>(f), std::move(p)));
}

template<typename From>
template<typename Function, typename To>
typename std::enable_if<!std::is_void<To>::value, Future<To>>::type
FutureBase<From>::then(Function &&f) && {
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    std::move(*this).then(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename Function>
class Mapper {

private:

    Function mFunction;

public:

    template<typename F>
    Mapper(F &&function) : mFunction(std::forward<F>(function)) { }

    template<typename From>
    auto operator()(common::Try<From> &&r)
            -> decltype(mFunction(std::move(*r))) {
        return mFunction(std::move(*r));
    }

};

template<typename From>
template<typename Function, typename To>
void FutureBase<From>::map(Function &&f, Promise<To> &&p) && {
    using M = Mapper<typename std::decay<Function>::type>;
    std::move(*this).then(M(std::forward<Function>(f)), std::move(p));
}

template<typename From>
template<typename Function, typename To>
Future<To> FutureBase<From>::map(Function &&f) && {
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    std::move(*this).map(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename Function>
class Recoverer {

private:

    Function mFunction;

public:

    template<typename F>
    Recoverer(F &&function) : mFunction(std::forward<F>(function)) { }

    template<typename T>
    T operator()(common::Try<T> &&r) {
        if (r.hasValue())
            return std::move(*r);
        return mFunction(r.template value<std::exception_ptr>());
    }

};

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value>::type
FutureBase<T>::recover(F &&function, Promise<T> &&p) && {
    using R = Recoverer<typename std::decay<F>::type>;
    std::move(*this).then(R(std::forward<F>(function)), std::move(p));
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value, Future<T>>::type
FutureBase<T>::recover(F &&function) && {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).recover(std::forward<F>(function), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void FutureBase<T>::forward(Promise<T> &&receiver) && {
    std::move(*this).map(common::Identity(), std::move(receiver));
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
void FutureBase<T>::wrap(Promise<Future<T>> &&p) && {
    std::move(*this).map(createFutureOf<T>, std::move(p));
}

template<typename T>
Future<Future<T>> FutureBase<T>::wrap() && {
    std::pair<Promise<Future<T>>, Future<Future<T>>> pf =
            createPromiseFuturePair<Future<T>>();
    std::move(*this).wrap(std::move(pf.first));
    return std::move(pf.second);
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
void Future<Future<T>>::unwrap(Promise<T> &&p) && {
    std::move(*this).then(
            common::SharedFunction<Unwrapper<T>>::create(std::move(p)));
}

template<typename T>
Future<T> Future<Future<T>>::unwrap() && {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).unwrap(std::move(pf.first));
    return std::move(pf.second);
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Future_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
