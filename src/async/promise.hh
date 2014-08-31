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

#ifndef INCLUDED_async_promise_hh
#define INCLUDED_async_promise_hh

#include "buildconfig.h"

#include <exception>
#include <functional>
#include <utility>
#include "async/delay_holder.hh"
#include "common/functional_initialize.hh"
#include "common/type_tag.hh"

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
class promise : public delay_holder<T> {

public:

    using delay_holder<T>::delay_holder;

    /**
     * Sets the result of the associated future by constructing T with the
     * arguments. If the constructor throws, the result is set to the exception
     * thrown. After the result is set, this promise will have no associated
     * future. Then, if the future has already a callback set, it is called
     * with the result.
     *
     * The behavior is undefined if this promise has no associated future.
     */
    template<typename... Arg>
    void set_result(Arg &&... arg) && {
        promise copy = std::move(*this);
        copy.delay().set_result(
                common::type_tag<T>(), std::forward<Arg>(arg)...);
    }

    /**
     * Sets the result of the associated future to the result of the argument
     * function, which must be callable with no arguments and return a value of
     * T. If the function throws, the result is set to the exception thrown.
     * After the result is set, this promise will have no associated future.
     * Then, if the future has already a callback set, it is called with the
     * result.
     *
     * The behavior is undefined if this promise has no associated future.
     */
    template<typename F>
    void set_result_from(F &&f) && {
        promise copy = std::move(*this);
        copy.delay().set_result(
                common::functional_initialize(), std::forward<F>(f));
    }

    /**
     * Sets the result of the associated future to the given exception. After
     * the result is set, this promise will have no associated future. Then, if
     * the future has already a callback set, it is called with the result.
     *
     * The behavior is undefined if this promise has no associated future or if
     * the exception pointer is null.
     */
    void fail(const std::exception_ptr &e) && {
        promise copy = std::move(*this);
        copy.delay().set_result(e);
    }

    /**
     * Sets the result of the associated future to the current exception. After
     * the result is set, this promise will have no associated future. Then, if
     * the future has already a callback set, it is called with the result.
     *
     * This function can be used in a catch clause only. The behavior is
     * undefined if this promise has no associated future.
     */
    void fail_with_current_exception() && {
        std::move(*this).fail(std::current_exception());
    }

}; // template<typename T> class promise

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_promise_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
