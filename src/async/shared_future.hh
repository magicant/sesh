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

#ifndef INCLUDED_async_shared_future_hh
#define INCLUDED_async_shared_future_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include "async/future.hh"

namespace sesh {
namespace async {

namespace future_impl {

template<typename T>
class shared_future;

template<typename T>
class shared_future_base {

private:

    class impl;

    /** May be null. */
    std::shared_ptr<impl> m_impl;

public:

    /** Creates a new invalid shared future. */
    shared_future_base() = default;

    /**
     * Creates a shared future from a future.
     *
     * If the argument future is not valid, the resulting shared future is not
     * valid, either.
     */
    shared_future_base(future<T> &&);

    /**
     * Compares two shared futures. The two are compared equal if and only if
     * they share the same associated promise.
     */
    bool operator==(const shared_future_base<T> &) const noexcept;

    /**
     * Compares two shared futures. The two are compared equal if and only if
     * they share the same associated promise.
     */
    bool operator!=(const shared_future_base<T> &) const noexcept;

    /**
     * Returns true if and only if this shared future is valid, that is,
     * associated with a promise. Callback functions can be added to valid
     * shared futures only.
     */
    bool is_valid() const noexcept;

    /** True if this future is valid. */
    explicit operator bool() const noexcept;

    /**
     * Adds a callback function to receive the result from the associated
     * promise.
     *
     * The behavior is undefined if this future instance has no associated
     * promise.
     *
     * @tparam F Type of the callback function. It must return void when called
     * with an argument of type <code>const common::trial<T> &</code>.
     */
    template<typename F>
    typename std::enable_if<std::is_void<typename std::result_of<
            typename std::decay<F>::type(const common::trial<T> &)
    >::type>::value>::type
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
     * argument of type <code>const common::trial<T> &</code>.
     * @tparam R Result type of the callback.
     */
    template<typename F, typename R>
    void then(F &&, promise<R> &&) const;

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
     * argument of type <code>const common::trial<T> &</code>.
     * @tparam R Result type of the callback. Must not be void.
     */
    template<
            typename F,
            typename R = typename std::result_of<
                typename std::decay<F>::type(const common::trial<T> &)>::type>
    typename std::enable_if<!std::is_void<R>::value, future<R>>::type
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
     * argument of type <code>const T &</code>.
     * @tparam R Result type of the callback.
     */
    template<typename F, typename R>
    void map(F &&, promise<R> &&) const;

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
     * argument of type <code>const T &</code>.
     * @tparam R Result type of the callback.
     */
    template<
            typename F,
            typename R = typename std::result_of<
                    typename std::decay<F>::type(const T &)>::type>
    future<R> map(F &&) const;

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
    recover(F &&, promise<T> &&) const;

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
    >::value, future<T>>::type
    recover(F &&) const;

    /**
     * Adds a callback to this future so that its result will be set to the
     * argument promise.
     */
    void forward(promise<T> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another future that is set to the argument promise.
     *
     * If this future receives an exception, it is directly propagated to the
     * argument promise, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    void wrap(promise<future<T>> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another future that is set to the returned future.
     *
     * If this future receives an exception, it is directly propagated to the
     * returned future, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    future<future<T>> wrap() const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another shared future that is set to the argument promise.
     *
     * If this future receives an exception, it is directly propagated to the
     * argument promise, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    void wrap_shared(promise<shared_future<T>> &&) const;

    /**
     * Adds a callback function to this future so that its result is wrapped in
     * another shared future that is set to the returned future.
     *
     * If this future receives an exception, it is directly propagated to the
     * returned future, not to the inner future. If the copy-constructor of
     * the result throws an exception, it is set to the inner future.
     */
    future<shared_future<T>> wrap_shared() const;

}; // template<typename> shared_future_base

/**
 * A shared future is a future that accepts multiple callback functions.
 *
 * All callbacks receives const references to the same result object.
 *
 * The template argument may be an incomplete type when the template is
 * instantiated.
 */
template<typename T>
class shared_future : public shared_future_base<T> {

public:

    using shared_future_base<T>::shared_future_base;

}; // template<typename> class shared_future

/** A specialization of the future class that has the unwrap method. */
template<typename T>
class future<shared_future<T>> : public future_base<shared_future<T>> {

public:

    using future_base<shared_future<T>>::future_base;

    /**
     * Unwraps this nested future. The argument promise will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    void unwrap(promise<T> &&) &&;

    /**
     * Unwraps this nested future. The returned future will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    future<T> unwrap() &&;

}; // template<typename T> class shared_future<future<T>>

/** A specialization of the future class that has the unwrap method. */
template<typename T>
class shared_future<shared_future<T>> :
        public shared_future_base<shared_future<T>> {

public:

    using shared_future_base<shared_future<T>>::shared_future_base;

    /**
     * Unwraps this nested future. The argument promise will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    void unwrap(promise<T> &&) const;

    /**
     * Unwraps this nested future. The returned future will receive the same
     * result as the inner future. If either future is invalid, the behavior is
     * undefined.
     */
    future<T> unwrap() const;

}; // template<typename T> class shared_future<future<T>>

template<typename T>
bool operator==(const shared_future_base<T> &, std::nullptr_t);

template<typename T>
bool operator!=(const shared_future_base<T> &, std::nullptr_t);

template<typename T>
bool operator==(std::nullptr_t, const shared_future_base<T> &);

template<typename T>
bool operator!=(std::nullptr_t, const shared_future_base<T> &);

} // namespace future_impl

using future_impl::shared_future;

} // namespace async
} // namespace sesh

#include "shared_future.tcc"

#endif // #ifndef INCLUDED_async_shared_future_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
