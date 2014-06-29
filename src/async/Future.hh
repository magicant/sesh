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

#ifndef INCLUDED_async_Future_hh
#define INCLUDED_async_Future_hh

#include "buildconfig.h"

#include <exception>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include "async/Delay.hh"
#include "async/DelayHolder.hh"
#include "async/Promise.hh"
#include "common/Try.hh"

namespace sesh {
namespace async {

namespace future_impl {

template<typename T>
class Future;

/** A base class that implements most part of the future class. */
template<typename T>
class FutureBase : public DelayHolder<T> {

public:

    using Callback = typename Delay<T>::Callback;

    using DelayHolder<T>::DelayHolder;

    /**
     * Sets a callback function to receive the result from the associated
     * promise. After the callback is set, this future instance will have no
     * associated promise.
     *
     * The behavior is undefined if this future instance has no associated
     * promise.
     */
    void setCallback(Callback &&) &&;

    /**
     * Sets a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result,
     * either successful or unsuccessful. The result of the callback will be
     * set to the argument promise. If the callback throws an exception, that
     * will be propagated to the promise.
     *
     * @tparam F type of the callback function
     * @tparam R result type of the callback
     */
    template<typename F, typename R>
    void then(F &&, Promise<R> &&) &&;

    /**
     * Sets a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result,
     * either successful or unsuccessful. The result of the callback will
     * become the result of the future returned from this method. If the
     * callback throws an exception, that will be propagated to the returned
     * future.
     *
     * @tparam F type of the callback function
     * @tparam R Result type of the callback. Must not be void.
     */
    template<
            typename F,
            typename R = typename std::result_of<
                    typename std::decay<F>::type(common::Try<T> &&)>::type>
    typename std::enable_if<!std::is_void<R>::value, Future<R>>::type
    then(F &&) &&;

    /**
     * Sets a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result.
     * The result of the callback will be set to the argument promise.
     *
     * If this future receives an exception, the callback will not be called
     * and the exception will be propagated to the promise.
     *
     * If the callback throws an exception, that will be propagated alike.
     *
     * @tparam F type of the callback function
     * @tparam R result type of the callback
     */
    template<typename F, typename R>
    void map(F &&, Promise<R> &&) &&;

    /**
     * Sets a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result.
     * The result of the callback will become the result of the future returned
     * from this method.
     *
     * If this future receives an exception, the callback will not be called
     * and the exception will be propagated to the future returned from this
     * method.
     *
     * If the callback throws an exception, that will be propagated alike.
     *
     * @tparam F type of the callback function
     * @tparam R result type of the callback
     */
    template<
            typename F,
            typename R = typename std::result_of<
                    typename std::decay<F>::type(T &&)>::type>
    Future<R> map(F &&) &&;

    /**
     * Sets a callback function that recovers this future from an exception.
     *
     * If this future receives a result of type T, then the argument callback
     * will not be called and the result is simply set to the argument promise.
     *
     * If this future returned an exception, the callback is called with a
     * pointer to the exception. The result of the callback will be set to the
     * promise.
     *
     * If the callback function or the move-constructor of the result throws an
     * exception, that will be propagated to the promise.
     *
     * @tparam F Parameter type of the callback function. It must be callable
     * with an exception pointer parameter and return a result of type T.
     */
    template<typename F>
    typename std::enable_if<std::is_same<
            T, typename std::result_of<F(std::exception_ptr)>::type
    >::value>::type
    recover(F &&, Promise<T> &&) &&;

    /**
     * Sets a callback function that recovers this future from an exception.
     *
     * If this future receives a result of type T, then the argument callback
     * will not be called and the future returned from this method will receive
     * the same result.
     *
     * If this future returned an exception, the callback is called with a
     * pointer to the exception. The result of the callback will become the
     * result of the future returned from this method.
     *
     * If the callback function or the move-constructor of the result throws an
     * exception, that will be propagated to the returned future.
     *
     * @tparam F Parameter type of the callback function. It must be callable
     * with an exception pointer parameter and return a result of type T.
     */
    template<typename F>
    typename std::enable_if<std::is_same<
            T, typename std::result_of<F(std::exception_ptr)>::type
    >::value, Future<T>>::type
    recover(F &&) &&;

    /**
     * Adds a callback to this future so that its result will be set to the
     * argument promise.
     */
    void forward(Promise<T> &&) &&;

    /**
     * Sets a callback function to this future so that its result is wrapped in
     * another future that is set to the argument promise.
     *
     * If this future receives an exception, it is directly propagated to the
     * argument promise, not to the inner future. If the move-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    void wrap(Promise<Future<T>> &&) &&;

    /**
     * Sets a callback function to this future so that its result is wrapped in
     * another future that is set to the returned future.
     *
     * If this future receives an exception, it is directly propagated to the
     * returned future, not to the inner future. If the move-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    Future<Future<T>> wrap() &&;

}; // template<typename T> class FutureBase

/**
 * The output side of a future/promise pair.
 *
 * You can set a callback function to the future to asynchronously receive the
 * result of computation from the associated promise.
 *
 * A future instance has no associated promise if it was default-constructed or
 * a callback has been set.
 *
 * Futures are not copyable to prevent setting multiple callbacks. The shared
 * future class, however, allows setting multiple callbacks for a single
 * promise.
 *
 * @tparam T The result type. It must be a decayed type other than
 * std::exception_ptr.
 */
template<typename T>
class Future : public FutureBase<T> {

public:

    using FutureBase<T>::FutureBase;

}; // template<typename T> class Future

/** A specialization of the future class that has the unwrap method. */
template<typename T>
class Future<Future<T>> : public FutureBase<Future<T>> {

public:

    using FutureBase<Future<T>>::FutureBase;

    /**
     * Unwraps this nested future. The argument promise will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    void unwrap(Promise<T> &&) &&;

    /**
     * Unwraps this nested future. The returned future will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    Future<T> unwrap() &&;

}; // template<typename T> class Future<Future<T>>

/**
 * Creates a pair of new promise and future that are associated with each
 * other.
 */
template<typename T>
std::pair<Promise<T>, Future<T>> createPromiseFuturePair();

/**
 * Constructs a future that has already received a result from the argument
 * function. The argument function is called to construct the result of the
 * returned future. If the argument function throws an exception, it is set to
 * the returned future.
 *
 * @tparam F type of the argument function that returns a value (or throws an
 * exception) that becomes the result of the returned future.
 */
template<typename F>
auto createFutureFrom(F &&) -> Future<typename std::result_of<F()>::type>;

/**
 * Constructs a future that has already received a result constructed from
 * the arguments. If the constructor of the result throws an exception, it
 * is set to the returned future.
 *
 * @tparam T type of the result of the returned future.
 * @tparam Arg argument types.
 * @param arg arguments that are passed to the constructor of T.
 */
template<typename T, typename... Arg>
Future<T> createFuture(Arg &&... arg);

/**
 * Constructs a future whose result has been set to a copy of the argument. If
 * the copy- or move-constructor of the result throws an exception, it is set
 * to the returned future.
 */
template<typename T>
auto createFutureOf(T &&) -> Future<typename std::decay<T>::type>;

/** Constructs a future whose result has been set to the argument exception. */
template<typename T>
Future<T> createFailedFuture(std::exception_ptr);

/** Constructs a future whose result has been set to the argument exception. */
template<typename T, typename E>
Future<T> createFailedFutureOf(E &&e);

} // namespace future_impl

using future_impl::Future;
using future_impl::createFailedFuture;
using future_impl::createFailedFutureOf;
using future_impl::createFuture;
using future_impl::createFutureFrom;
using future_impl::createFutureOf;
using future_impl::createPromiseFuturePair;

} // namespace async
} // namespace sesh

#include "Future.tcc"

#endif // #ifndef INCLUDED_async_Future_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
