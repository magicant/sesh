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

namespace sesh {
namespace async {

/**
 * Communicator between a future and promise.
 *
 * It may contain a result and a callback function, both of which are initially
 * empty. They can independently set non-empty afterward. When both are set,
 * the callback function is called with the result passed as an argument.
 * Neither the result nor callback can be set twice.
 *
 * The result and callback set are not destroyed until this delay object is
 * destroyed. This is because the delay object would normally be destroyed by
 * the client just after the result is passed to the callback, so the result
 * and callback are soon destroyed anyway.
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
     * Sets the result of this delay object by constructing Try&lt;T> with the
     * arguments. If the constructor throws, the result is set to the exception
     * thrown.
     *
     * The behavior is undefined if the result has already been set.
     *
     * If a callback function has already been set, it is called immediately
     * with the result set.
     */
    template<typename... Arg>
    void setResult(Arg &&... arg) {
        assert(!mResult.hasValue());

        try {
            mResult.emplace(std::forward<Arg>(arg)...);
        } catch (...) {
            mResult.emplace(std::current_exception());
        }

        fireIfReady();
    }

    /**
     * Sets a callback function to this delay object.
     *
     * The behavior is undefined if a callback has already been set or the
     * argument callback is empty.
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
