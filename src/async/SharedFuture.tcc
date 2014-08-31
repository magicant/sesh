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
#include "async/future.tcc"
#include "common/identity.hh"
#include "common/maybe.hh"
#include "common/trial.hh"

namespace sesh {
namespace async {
namespace future_impl {

template<typename T>
class SharedFutureBase<T>::Impl {

public:

    using Callback = std::function<void(const common::trial<T> &)>;

private:

    common::maybe<common::trial<T>> mResult;

    /** Empty after the result is set and all callbacks are called. */
    std::vector<Callback> mCallbacks;

public:

    void setResult(common::trial<T> &&);

    void addCallback(Callback &&);

}; // template<typename T> class SharedFuture<T>::Impl

template<typename T>
void SharedFutureBase<T>::Impl::setResult(common::trial<T> &&t) {
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
SharedFutureBase<T>::SharedFutureBase(future<T> &&f) : mImpl() {
    if (!f.is_valid())
        return;

    mImpl = std::make_shared<Impl>();

    auto &impl = mImpl;
    std::move(f).then([impl](common::trial<T> &&t) {
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
typename std::enable_if<std::is_void<typename std::result_of<
        typename std::decay<Function>::type(const common::trial<T> &)
>::type>::value>::type
SharedFutureBase<T>::then(Function &&f) const {
    mImpl->addCallback(std::forward<Function>(f));
}

template<typename From>
template<typename Function, typename To>
void SharedFutureBase<From>::then(Function &&f, promise<To> &&p) const {
    using C = composer<To, typename std::decay<Function>::type>;
    then(common::shared_function<C>::create(
            std::forward<Function>(f), std::move(p)));
}

template<typename From>
template<typename Function, typename To>
typename std::enable_if<!std::is_void<To>::value, future<To>>::type
SharedFutureBase<From>::then(Function &&f) const {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    then(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename From>
template<typename Function, typename To>
void SharedFutureBase<From>::map(Function &&f, promise<To> &&p) const {
    using M = mapper<typename std::decay<Function>::type>;
    then(M(std::forward<Function>(f)), std::move(p));
}

template<typename From>
template<typename Function, typename To>
future<To> SharedFutureBase<From>::map(Function &&f) const {
    std::pair<promise<To>, future<To>> pf = make_promise_future_pair<To>();
    map(std::forward<Function>(f), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value>::type
SharedFutureBase<T>::recover(F &&function, promise<T> &&p) const {
    using R = recoverer<typename std::decay<F>::type>;
    then(R(std::forward<F>(function)), std::move(p));
}

template<typename T>
template<typename F>
typename std::enable_if<std::is_same<
        T, typename std::result_of<F(std::exception_ptr)>::type
>::value, future<T>>::type
SharedFutureBase<T>::recover(F &&function) const {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    recover(std::forward<F>(function), std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFutureBase<T>::forward(promise<T> &&receiver) const {
    map(common::identity(), std::move(receiver));
}

template<typename T>
void SharedFutureBase<T>::wrap(promise<future<T>> &&p) const {
    map(make_future_of<const T &>, std::move(p));
}

template<typename T>
future<future<T>> SharedFutureBase<T>::wrap() const {
    using F = future<T>;
    std::pair<promise<F>, future<F>> pf = make_promise_future_pair<F>();
    wrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFutureBase<T>::wrapShared(promise<SharedFuture<T>> &&p) const {
    auto wrapper = [](const T &v) -> SharedFuture<T> {
        return make_future_of<const T &>(v);
    };
    map(wrapper, std::move(p));
}

template<typename T>
future<SharedFuture<T>> SharedFutureBase<T>::wrapShared() const {
    using F = SharedFuture<T>;
    std::pair<promise<F>, future<F>> pf = make_promise_future_pair<F>();
    wrapShared(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
class SharedUnwrapper {

private:

    promise<T> mReceiver;

public:

    explicit SharedUnwrapper(promise<T> &&receiver) noexcept :
            mReceiver(std::move(receiver)) { }

    void operator()(const common::trial<SharedFuture<T>> &r) {
        if (r.has_value())
            return r->forward(std::move(mReceiver));

        std::move(mReceiver).fail(r.template value<std::exception_ptr>());
    }

};

template<typename T>
void future<SharedFuture<T>>::unwrap(promise<T> &&p) && {
    std::move(*this).then(
            common::shared_function<SharedUnwrapper<T>>::create(std::move(p)));
}

template<typename T>
future<T> future<SharedFuture<T>>::unwrap() && {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
    std::move(*this).unwrap(std::move(pf.first));
    return std::move(pf.second);
}

template<typename T>
void SharedFuture<SharedFuture<T>>::unwrap(promise<T> &&p) const {
    this->then(
            common::shared_function<SharedUnwrapper<T>>::create(std::move(p)));
}

template<typename T>
future<T> SharedFuture<SharedFuture<T>>::unwrap() const {
    std::pair<promise<T>, future<T>> pf = make_promise_future_pair<T>();
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
