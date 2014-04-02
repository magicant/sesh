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

#ifndef INCLUDED_async_Delay_hh
#define INCLUDED_async_Delay_hh

#include "buildconfig.h"

#include <cassert>
#include <exception>
#include <functional>
#include <utility>
#include "common/Maybe.hh"
#include "common/Try.hh"
#include "common/Variant.hh"

namespace sesh {
namespace async {

/**
 * Communicator between a future and promise.
 *
 * It may contain a result and a callback function, both of which are initially
 * empty. When the both is first set non-empty, the callback function is
 * called. Either the result and callback must not be set twice.
 *
 * The result and callback set are not destroyed until this delay object is
 * destroyed.
 *
 * @tparam T The result type. It must be a decayed move-constructible type
 * other than std::exception_ptr.
 */
template<typename T>
class Delay {

public:

    using Callback = std::function<void(common::Try<T> &&)>;

private:

    common::Maybe<common::Try<T>> mResult;
    Callback mCallback;

    void fireIfReady() {
        if (mResult.hasValue() && mCallback != nullptr)
            mCallback(std::move(mResult.value()));
    }

public:

    /**
     * Sets the result of this delay object to the result of a call to the
     * argument function, which must be callable with no arguments and return a
     * value of T. If the function throws, the result is set to the exception
     * thrown.
     *
     * The behavior is undefined if the result has already been set.
     *
     * If a callback function has already been set, it is called immediately
     * with the result set.
     */
    template<typename F>
    void setResultFrom(F &&f) {
        assert(!mResult.hasValue());

        try {
            mResult.emplace(std::forward<F>(f));
        } catch (...) {
            mResult.emplace(common::Try<T>::of(std::current_exception()));
        }

        fireIfReady();
    }

    /**
     * Sets a callback function to this delay object.
     *
     * The behavior is undefined if a callback has already been set.
     *
     * If a result has already been set, the callback function is called
     * immediately with the result.
     */
    void setCallback(Callback &&f) {
        assert(mCallback == nullptr);
        assert(f != nullptr);

        mCallback = std::move(f);
        fireIfReady();
    }

}; // template<typename T> class Delay

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Delay_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
