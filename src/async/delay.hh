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

#ifndef INCLUDED_async_delay_hh
#define INCLUDED_async_delay_hh

#include "buildconfig.h"

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <utility>
#include "async/continuation.hh"
#include "common/direct_initialize.hh"
#include "common/either.hh"
#include "common/empty.hh"
#include "common/function_helper.hh"
#include "common/type_tag.hh"

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
 * A delay object must be allocated in the heap and managed by std::shared_ptr.
 *
 * @tparam T The result type. It must be a decayed move-constructible type
 * other than std::exception_ptr.
 */
template<typename T>
class delay : public runnable, public std::enable_shared_from_this<delay<T>> {

public:

    class callback {

    private:

        virtual continuation do_call(common::trial<T> &&) noexcept = 0;

    public:

        virtual ~callback() = default;

        continuation operator()(common::trial<T> &&t) noexcept {
            return do_call(std::move(t));
        }

    }; // class callback

    template<typename F>
    class callback_wrapper : public callback {

    public:

        using value_type = F;

    private:

        F m_function;

    public:

        template<typename... A>
        callback_wrapper(A &&... a)
                noexcept(std::is_nothrow_constructible<F, A...>::value) :
                m_function(std::forward<A>(a)...) { }

        continuation do_call(common::trial<T> &&t) noexcept final override {
            return call(m_function, std::move(t));
        }

    }; // template<typename F> class callback_wrapper

    using callback_pointer = std::unique_ptr<callback>;

private:

    using empty = common::empty;
    using trial = common::trial<T>;
    using forward_source = std::weak_ptr<delay>;
    using forward_target = std::shared_ptr<delay>;

    using input = common::variant<empty, trial, forward_source>;
    using output = common::variant<empty, callback_pointer, forward_target>;

    input m_input = input(empty());
    output m_output = output(empty());

    continuation to_continuation() {
        if (m_input.tag() != m_input.template tag<trial>())
            return {};
        if (m_output.tag() != m_output.template tag<callback_pointer>())
            return {};
        return continuation(this->shared_from_this());
    }

    continuation do_run() noexcept final override {
        auto &f = m_output.template value<callback_pointer>();
        return (*f)(std::move(m_input.template value<trial>()));
    }

public:

    /**
     * Sets the result of this delay object by constructing
     * <code>trial&lt;T></code> with the arguments. If the constructor throws,
     * the result is set to the exception thrown.
     *
     * The behavior is undefined if the result has already been set.
     *
     * @return Continuation that must be resumed immediately after returning
     * from this function. Note that the continuation destructor automatically
     * resumes it so normally you can simply ignore the return value. If the
     * result and callback are both set to the delay, the continuation calls
     * the callback passing the result to it. Otherwise, the continuation is a
     * nop.
     */
    template<typename... Arg>
    continuation set_result(Arg &&... arg) {
        assert(m_input.tag() != m_input.template tag<trial>());

        if (m_output.tag() == m_output.template tag<forward_target>())
            return m_output.template value<forward_target>()->set_result(
                    std::forward<Arg>(arg)...);

        try {
            m_input.template emplace_with_fallback<empty>(
                    common::direct_initialize(),
                    common::type_tag<trial>(),
                    std::forward<Arg>(arg)...);
        } catch (...) {
            m_input.emplace(
                    common::direct_initialize(),
                    common::type_tag<trial>(),
                    std::current_exception());
        }

        return to_continuation();
    }

    /**
     * Sets a callback function to this delay object.
     *
     * The behavior is undefined if a callback has already been set or the
     * argument callback is empty.
     *
     * @return Continuation that must be resumed immediately after returning
     * from this function. Note that the continuation destructor automatically
     * resumes it so normally you can simply ignore the return value. If the
     * result and callback are both set to the delay, the continuation calls
     * the callback passing the result to it. Otherwise, the continuation is a
     * nop.
     */
    continuation set_callback(callback_pointer &&f) {
        assert(m_output.tag() != m_output.template tag<callback_pointer>());
        assert(f != nullptr);

        if (m_input.tag() == m_input.template tag<forward_source>()) {
            if (auto fs = m_input.template value<forward_source>().lock())
                return fs->set_callback(std::move(f));
            return {};
        }

        m_output.emplace(std::move(f));

        return to_continuation();
    }

    /**
     * Sets a callback function to this delay object.
     *
     * The behavior is undefined if a callback has already been set or the
     * argument callback is empty.
     *
     * @tparam F Type of the callback. Must be copy- or move-constructible (if
     * passed by l- or r-value reference, respectively) and must return
     * continuation when called with <code>common::trial&lt;T></code>.
     * @param a Parameters that are passed to the constructor of @c F.
     *
     * @return Continuation that must be resumed immediately after returning
     * from this function. Note that the continuation destructor automatically
     * resumes it so normally you can simply ignore the return value. If the
     * result and callback are both set to the delay, the continuation calls
     * the callback passing the result to it. Otherwise, the continuation is a
     * nop.
     */
    template<typename F, typename... A>
    continuation emplace_callback(A &&... a) {
        return set_callback(callback_pointer(
                new callback_wrapper<F>(std::forward<A>(a)...)));
    }

    /**
     * Sets a callback function to this delay object.
     *
     * The behavior is undefined if a callback has already been set or the
     * argument callback is empty.
     *
     * @tparam F Type of the callback. <code>std::delay&lt;F>::type</code> must
     * be copy- or move-constructible (if passed by l- or r-value reference,
     * respectively) and must return continuation when called with
     * <code>common::trial&lt;T></code>.
     *
     * @return Continuation that must be resumed immediately after returning
     * from this function. Note that the continuation destructor automatically
     * resumes it so normally you can simply ignore the return value. If the
     * result and callback are both set to the delay, the continuation calls
     * the callback passing the result to it. Otherwise, the continuation is a
     * nop.
     */
    template<typename F>
    continuation set_callback(F &&f) {
        return emplace_callback<typename std::decay<F>::type>(
                std::forward<F>(f));
    }

    /**
     * Connects two delay objects as if a callback is set to the "from" object
     * so that the result set to the "from" object is simply transferred to the
     * "to" object.
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
     * function, {@link #set_callback} and {@link #set_result} must not be
     * called for the "from" and "to" objects, respectively.
     *
     * @return Continuation that must be resumed immediately after returning
     * from this function. Note that the continuation destructor automatically
     * resumes it so normally you can simply ignore the return value. If the
     * result and callback are both set to the connected delays, the
     * continuation calls the callback passing the result to it. Otherwise, the
     * continuation is a nop.
     */
    static continuation forward(
            std::shared_ptr<delay> &&from, std::shared_ptr<delay> &&to) {
        assert(from != nullptr);
        assert(from->m_output.tag() !=
                from->m_output.template tag<callback_pointer>());

        assert(to != nullptr);
        assert(to->m_input.tag() != to->m_input.template tag<trial>());

        // Normalize "from"
        if (from->m_input.tag() ==
                from->m_input.template tag<forward_source>()) {
            from = from->m_input.template value<forward_source>().lock();
            if (from == nullptr)
                return {};
        }

        // Normalize "to"
        if (to->m_output.tag() == to->m_output.template tag<forward_target>())
            to = std::move(to->m_output.template value<forward_target>());

        // Transfer result
        if (from->m_input.tag() == from->m_input.template tag<trial>())
            return to->set_result(
                    std::move(from->m_input.template value<trial>()));

        // Transfer callback
        if (to->m_output.tag() ==
                to->m_output.template tag<callback_pointer>())
            return from->set_callback(std::move(
                    to->m_output.template value<callback_pointer>()));

        // Connect
        to->m_input.emplace(
                common::direct_initialize(),
                common::type_tag<forward_source>(),
                from);
        from->m_output.emplace(
                common::direct_initialize(),
                common::type_tag<forward_target>(),
                std::move(to));
        return {};
    }

}; // template<typename T> class delay

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_delay_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
