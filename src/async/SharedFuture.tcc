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

#ifndef INCLUDED_async_SharedFuture_tcc
#define INCLUDED_async_SharedFuture_tcc

#include "buildconfig.h"
#include "SharedFuture.hh"

#include <exception>
#include <utility>
#include "async/Future.tcc"
#include "common/Try.hh"
#include "common/identity.hh"
#include "common/maybe.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
class SharedFutureBase<T>::Impl {

public:

    using Callback = std::function<void(const common::Try<T> &)>;

private:

    common::maybe<common::Try<T>> mResult;

    /** Empty after the result is set and all callbacks are called. */
    std::vector<Callback> mCallbacks;

public:

    void setResult(common::Try<T> &&);

    void addCallback(Callback &&);

}; // template<typename T> class SharedFuture<T>::Impl

template<typename T>
void SharedFutureBase<T>::Impl::setResult(common::Try<T> &&t) {
    try {
        mResult.emplace(std::move(t));
    } catch (...) {
        mResult.emplace(std::current_exception());
    }

    for (Callback &c : mCallbacks)
        c(*mResult);
    mCallbacks.clear();
}

template<typename T>
void SharedFutureBase<T>::Impl::addCallback(Callback &&c) {
    if (mResult.has_value())
        return c(*mResult);
    mCallbacks.push_back(std::move(c));
}

template<typename T>
SharedFutureBase<T>::SharedFutureBase(Future<T> &&f) : mImpl() {
    if (!f.isValid())
        return;

    mImpl = std::make_shared<Impl>();

    auto &impl = mImpl;
    std::move(f).then([impl](common::Try<T> &&t) {
        impl->setResult(std::move(t));
    });
}

template<typename T>
bool SharedFutureBase<T>::operator==(const SharedFutureBase<T> &other) const
        noexcept {
    return this->mImpl == other.mImpl;
}

template<typename T>
bool SharedFutureBase<T>::operator!=(const SharedFutureBase<T> &other) const
        noexcept {
    return !(*this == other);
}

template<typename T>
bool SharedFutureBase<T>::isValid() const noexcept {
    return mImpl != nullptr;
}

template<typename T>
SharedFutureBase<T>::operator bool() const noexcept {
    return isValid();
}

template<typename T>
template<typename Function>
typename std::enable_if<std::is_void<
        typename std::result_of<
            typename std::decay<Function>::type(const common::Try<T> &)>::type
>::value>::type
SharedFutureBase<T>::then(Function &&f) const {
    mImpl->addCallback(std::forward<Function>(f));
}

template<typename From>
template<typename Function, typename To>
void SharedFutureBase<From>::then(Function &&f, Promise<To> &&p) const {
    using C = Composer<To, typename std::decay<Function>::type>;
    then(common::SharedFunction<C>::create(
            std::forward<Function>(f), std::move(p)));
}

template<typename From>
template<typename Function, typename To>
typename std::enable_if<!std::is_void<To>::value, Future<To>>::type
SharedFutureBase<From>::then(Function &&f) const {
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    then(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename From>
template<typename Function, typename To>
void SharedFutureBase<From>::map(Function &&f, Promise<To> &&p) const {
    using M = Mapper<typename std::decay<Function>::type>;
    then(M(std::forward<Function>(f)), std::move(p));
}

template<typename From>
template<typename Function, typename To>
Future<To> SharedFutureBase<From>::map(Function &&f) const {
    std::pair<Promise<To>, Future<To>> pf = createPromiseFuturePair<To>();
    map(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value>::type
SharedFutureBase<T>::recover(F &&function, Promise<T> &&p) const {
    using R = Recoverer<typename std::decay<F>::type>;
    then(R(std::forward<F>(function)), std::move(p));
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value, Future<T>>::type
SharedFutureBase<T>::recover(F &&function) const {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    recover(std::forward<F>(function), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFutureBase<T>::forward(Promise<T> &&receiver) const {
    map(common::identity(), std::move(receiver));
}

template<typename T>
void SharedFutureBase<T>::wrap(Promise<Future<T>> &&p) const {
    map(createFutureOf<const T &>, std::move(p));
}

template<typename T>
Future<Future<T>> SharedFutureBase<T>::wrap() const {
    using F = Future<T>;
    std::pair<Promise<F>, Future<F>> pf = createPromiseFuturePair<F>();
    wrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFutureBase<T>::wrapShared(Promise<SharedFuture<T>> &&p) const {
    auto wrapper = [](const T &v) -> SharedFuture<T> {
        return createFutureOf<const T &>(v);
    };
    map(wrapper, std::move(p));
}

template<typename T>
Future<SharedFuture<T>> SharedFutureBase<T>::wrapShared() const {
    using F = SharedFuture<T>;
    std::pair<Promise<F>, Future<F>> pf = createPromiseFuturePair<F>();
    wrapShared(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
class SharedUnwrapper {

private:

    Promise<T> mReceiver;

public:

    explicit SharedUnwrapper(Promise<T> &&receiver) noexcept :
            mReceiver(std::move(receiver)) { }

    void operator()(const common::Try<SharedFuture<T>> &r) {
        if (r.hasValue())
            return r->forward(std::move(mReceiver));

        std::move(mReceiver).fail(r.template value<std::exception_ptr>());
    }

};

template<typename T>
void Future<SharedFuture<T>>::unwrap(Promise<T> &&p) && {
    std::move(*this).then(
            common::SharedFunction<SharedUnwrapper<T>>::create(std::move(p)));
}

template<typename T>
Future<T> Future<SharedFuture<T>>::unwrap() && {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    std::move(*this).unwrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFuture<SharedFuture<T>>::unwrap(Promise<T> &&p) const {
    this->then(
            common::SharedFunction<SharedUnwrapper<T>>::create(std::move(p)));
}

template<typename T>
Future<T> SharedFuture<SharedFuture<T>>::unwrap() const {
    std::pair<Promise<T>, Future<T>> pf = createPromiseFuturePair<T>();
    unwrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
bool operator==(const SharedFutureBase<T> &f, std::nullptr_t) {
    return !f.isValid();
}

template<typename T>
bool operator!=(const SharedFutureBase<T> &f, std::nullptr_t) {
    return f.isValid();
}

template<typename T>
bool operator==(std::nullptr_t, const SharedFutureBase<T> &f) {
    return !f.isValid();
}

template<typename T>
bool operator!=(std::nullptr_t, const SharedFutureBase<T> &f) {
    return f.isValid();
}

} // namespace future_impl
} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_SharedFuture_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
