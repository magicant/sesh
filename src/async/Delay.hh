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
#include <memory>
#include <utility>
#include "common/Empty.hh"
#include "common/Maybe.hh"
#include "common/Try.hh"
#include "common/TypeTag.hh"

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

    using Empty = common::Empty;
    using Try = common::Try<T>;
    using ForwardSource = std::weak_ptr<Delay>;
    using ForwardTarget = std::shared_ptr<Delay>;

    using Input = common::Variant<Empty, Try, ForwardSource>;
    using Output = common::Variant<Empty, Callback, ForwardTarget>;

    Input mInput = Input(common::TypeTag<Empty>());
    Output mOutput = Output(common::TypeTag<Empty>());

    void fireIfReady() {
        if (mInput.tag() != mInput.template tag<Try>())
            return;
        if (mOutput.tag() != mOutput.template tag<Callback>())
            return;

        auto &f = mOutput.template value<Callback>();
        f(std::move(mInput.template value<Try>()));
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
        assert(mInput.tag() != mInput.template tag<Try>());

        if (mOutput.tag() == mOutput.template tag<ForwardTarget>())
            return mOutput.template value<ForwardTarget>()->setResult(
                    std::forward<Arg>(arg)...);

        try {
            mInput.template emplaceWithFallback<Try, Empty>(
                    std::forward<Arg>(arg)...);
        } catch (...) {
            mInput.emplace(common::TypeTag<Try>(), std::current_exception());
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
        assert(mOutput.tag() != mOutput.template tag<Callback>());
        assert(f != nullptr);

        if (mInput.tag() == mInput.template tag<ForwardSource>()) {
            if (auto fs = mInput.template value<ForwardSource>().lock())
                fs->setCallback(std::move(f));
            return;
        }

        mOutput.template emplaceWithFallback<Callback, Empty>(std::move(f));

        fireIfReady();
    }

    /**
     * Connects two Delay objects as if a callback is set to the "from" object
     * so that the result set to the "from" object is simply transferred to the
     * "to" object. If a result and callback both have already been set, the
     * result is immediately passed to the callback.
     *
     * Using this function is more efficient than setting a callback normally.
     * Especially, when more than two delay objects are connected in a row with
     * this function, the two endpoints are directly connected so that the
     * intermediate delay objects are dropped and deallocated.
     *
     * For maximum efficiency, the argument shared pointers should be destroyed
     * (or reset) as soon as possible after this function returned.
     *
     * The argument pointers must be non-null. The "from" and "to" objects must
     * not have a callback and result set, respectively. After calling this
     * function, {@link #setCallback} and {@link #setResult} must not be called
     * for the "from" and "to" objects, respectively.
     */
    static void forward(
            std::shared_ptr<Delay> &&from, std::shared_ptr<Delay> &&to) {
        assert(from != nullptr);
        assert(from->mOutput.tag() != from->mOutput.template tag<Callback>());

        assert(to != nullptr);
        assert(to->mInput.tag() != to->mInput.template tag<Try>());

        // Normalize "from"
        if (from->mInput.tag() == from->mInput.template tag<ForwardSource>()) {
            from = from->mInput.template value<ForwardSource>().lock();
            if (from == nullptr)
                return;
        }

        // Normalize "to"
        if (to->mOutput.tag() == to->mOutput.template tag<ForwardTarget>())
            to = std::move(to->mOutput.template value<ForwardTarget>());

        // Transfer result
        if (from->mInput.tag() == from->mInput.template tag<Try>())
            return to->setResult(
                    std::move(from->mInput.template value<Try>()));

        // Transfer callback
        if (to->mOutput.tag() == to->mOutput.template tag<Callback>())
            return from->setCallback(
                    std::move(to->mOutput.template value<Callback>()));

        // Connect
        to->mInput.emplace(common::TypeTag<ForwardSource>(), from);
        from->mOutput.emplace(common::TypeTag<ForwardTarget>(), std::move(to));
    }

}; // template<typename T> class Delay

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Delay_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
