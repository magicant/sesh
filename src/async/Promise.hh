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

#ifndef INCLUDED_async_Promise_hh
#define INCLUDED_async_Promise_hh

#include "buildconfig.h"

#include <exception>
#include <functional>
#include <utility>
#include "async/DelayHolder.hh"

namespace sesh {
namespace async {

/**
 * The input side of a future/promise pair.
 *
 * You can set the result to the associated future through the promise. The
 * result can be set only once for each future.
 *
 * A promise instance has no associated future if it was default-constructed or
 * a result has been set.
 *
 * Promises are not copyable because a single future should not be associated
 * with more than one promise.
 *
 * @tparam T The result type. It must be a decayed type other than
 * std::exception_ptr.
 */
template<typename T>
class Promise : public DelayHolder<T> {

public:

    using DelayHolder<T>::DelayHolder;

    /**
     * Sets the result of the associated future by constructing T with the
     * arguments. If the constructor throws, the result is set to the exception
     * thrown. After the result is set, this promise will have no associated
     * future.
     *
     * The behavior is undefined if this promise has no associated future.
     */
    template<typename... Arg>
    void setResult(Arg &&... arg) {
        this->delay().setResult(std::forward<Arg>(arg)...);
        this->invalidate();
    }

    /**
     * Sets the result of the associated future to the result of the argument
     * function, which must be callable with no arguments and return a value of
     * T. If the function throws, the result is set to the exception thrown.
     * After the result is set, this promise will have no associated future.
     *
     * The behavior is undefined if this promise has no associated future.
     */
    template<typename F>
    void setResultFrom(F &&f) && {
        this->delay().setResultFrom(std::forward<F>(f));
        this->invalidate();
    }

    /**
     * Sets the result of the associated future to the given exception. The
     * behavior is undefined if the exception pointer is null.
     */
    void fail(const std::exception_ptr &e) && {
        this->delay().setResultException(e);
        this->invalidate();
    }

    /**
     * Sets the result of the associated future to the current exception. This
     * function can be used in a catch clause only.
     */
    void failWithCurrentException() && {
        std::move(*this).fail(std::current_exception());
    }

}; // template<typename T> class Promise

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Promise_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
