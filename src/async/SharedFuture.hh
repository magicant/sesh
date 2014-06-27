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

#ifndef INCLUDED_async_SharedFuture_hh
#define INCLUDED_async_SharedFuture_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include "async/Future.hh"

namespace sesh {
namespace async {

namespace future_impl {

template<typename T>
class SharedFuture;

template<typename T>
class SharedFutureBase {

private:

    class Impl;

    /** May be null. */
    std::shared_ptr<Impl> mImpl;

public:

    /** Creates a new invalid shared future. */
    SharedFutureBase() = default;

    /**
     * Creates a shared future from a future.
     *
     * If the argument future is not valid, the resulting shared future is not
     * valid, either.
     */
    SharedFutureBase(Future<T> &&);

    /**
     * Compares two shared futures. The two are compared equal if and only if
     * they share the same associated promise.
     */
    bool operator==(const SharedFutureBase<T> &) const noexcept;

    /**
     * Compares two shared futures. The two are compared equal if and only if
     * they share the same associated promise.
     */
    bool operator!=(const SharedFutureBase<T> &) const noexcept;

    /**
     * Returns true if and only if this shared future is valid, that is,
     * associated with a promise. Callback functions can be added to valid
     * shared futures only.
     */
    bool isValid() const noexcept;

    /** True if this future is valid. */
    operator bool() const noexcept;

    /**
     * Adds a callback function to receive the result from the associated
     * promise.
     *
     * The behavior is undefined if this future instance has no associated
     * promise.
     *
     * @tparam F Type of the callback function. It must return void when called
     * with an argument of type {@code const common::Try<T> &}.
     */
    template<typename F>
    typename std::enable_if<std::is_void<
            typename std::result_of<
                    typename std::decay<F>::type(const common::Try<T> &)>::type
    >::value>::type
    then(F &&) const;

    /**
     * Adds a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result,
     * either successful or unsuccessful. The result of the callback will be
     * set to the argument promise. If the callback throws an exception, that
     * will be propagated to the promise.
     *
     * @tparam F Type of the callback function. It must be callable with an
     * argument of type {@code const common::Try<T> &}.
     * @tparam R Result type of the callback.
     */
    template<typename F, typename R>
    void then(F &&, Promise<R> &&) const;

    /**
     * Adds a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result,
     * either successful or unsuccessful. The result of the callback will
     * become the result of the future returned from this method. If the
     * callback throws an exception, that will be propagated to the returned
     * future.
     *
     * @tparam F Type of the callback function. It must be callable with an
     * argument of type {@code const common::Try<T> &}.
     * @tparam R Result type of the callback. Must not be void.
     */
    template<
            typename F,
            typename R = typename std::result_of<
                typename std::decay<F>::type(const common::Try<T> &)>::type>
    typename std::enable_if<!std::is_void<R>::value, Future<R>>::type
    then(F &&) const;

    /**
     * Adds a callback function that converts the result to another result.
     *
     * The argument callback will be called when this future receives a result.
     * The result of the callback will be set to the argument promise.
     *
     * If this future receives an exception, the callback will not be called
     * and the exception will be propagated to the promise.
     *
     * If the callback throws an exception, that will be propagated alike.
     *
     * @tparam F Type of the callback function. It must be callable with an
     * argument of type {@code const T &}.
     * @tparam R Result type of the callback.
     */
    template<typename F, typename R>
    void map(F &&, Promise<R> &&) const;

    /**
     * Adds a callback function that converts the result to another result.
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
     * @tparam F Type of the callback function. It must be callable with an
     * argument of type {@code const T &}.
     * @tparam R Result type of the callback.
     */
    template<
            typename F,
            typename R = typename std::result_of<
                    typename std::decay<F>::type(const T &)>::type>
    Future<R> map(F &&) const;

    /**
     * Adds a callback function that recovers this future from an exception.
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
     * @tparam F Type of the callback function. It must be callable with an
     * exception pointer parameter and return a result of type T.
     */
    template<typename F>
    typename std::enable_if<std::is_same<
            T, typename std::result_of<F(std::exception_ptr)>::type
    >::value>::type
    recover(F &&, Promise<T> &&) const;

    /**
     * Adds a callback function that recovers this future from an exception.
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
     * @tparam F Type of the callback function. It must be callable with an
     * exception pointer parameter and return a result of type T.
     */
    template<typename F>
    typename std::enable_if<std::is_same<
            T, typename std::result_of<F(std::exception_ptr)>::type
    >::value, Future<T>>::type
    recover(F &&) const;

    /**
     * Adds a callback to this future so that its result will be set to the
     * argument promise.
     */
    void forward(Promise<T> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another future that is set to the argument promise.
     *
     * If this future receives an exception, it is directly propagated to the
     * argument promise, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    void wrap(Promise<Future<T>> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another future that is set to the returned future.
     *
     * If this future receives an exception, it is directly propagated to the
     * returned future, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    Future<Future<T>> wrap() const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another shared future that is set to the argument promise.
     *
     * If this future receives an exception, it is directly propagated to the
     * argument promise, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    void wrapShared(Promise<SharedFuture<T>> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another shared future that is set to the returned future.
     *
     * If this future receives an exception, it is directly propagated to the
     * returned future, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    Future<SharedFuture<T>> wrapShared() const;

}; // template<typename> SharedFutureBase

/**
 * A shared future is a future that accepts multiple callback functions.
 *
 * All callbacks receives const references to the same result object.
 */
template<typename T>
class SharedFuture : public SharedFutureBase<T> {

public:

    using SharedFutureBase<T>::SharedFutureBase;

}; // template<typename> class SharedFuture

/** A specialization of the future class that has the unwrap method. */
template<typename T>
class Future<SharedFuture<T>> : public FutureBase<SharedFuture<T>> {

public:

    using FutureBase<SharedFuture<T>>::FutureBase;

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

}; // template<typename T> class SharedFuture<Future<T>>

/** A specialization of the future class that has the unwrap method. */
template<typename T>
class SharedFuture<SharedFuture<T>> :
        public SharedFutureBase<SharedFuture<T>> {

public:

    using SharedFutureBase<SharedFuture<T>>::SharedFutureBase;

    /**
     * Unwraps this nested future. The argument promise will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    void unwrap(Promise<T> &&) const;

    /**
     * Unwraps this nested future. The returned future will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    Future<T> unwrap() const;

}; // template<typename T> class SharedFuture<Future<T>>

template<typename T>
bool operator==(const SharedFutureBase<T> &, std::nullptr_t);

template<typename T>
bool operator!=(const SharedFutureBase<T> &, std::nullptr_t);

template<typename T>
bool operator==(std::nullptr_t, const SharedFutureBase<T> &);

template<typename T>
bool operator!=(std::nullptr_t, const SharedFutureBase<T> &);

} // namespace future_impl

using future_impl::SharedFuture;

} // namespace async
} // namespace sesh

#include "SharedFuture.tcc"

#endif // #ifndef INCLUDED_async_SharedFuture_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
