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

#ifndef INCLUDED_async_continuation_hh
#define INCLUDED_async_continuation_hh

#include "buildconfig.h"

#include <memory>
#include <type_traits>
#include "common/function_helper.hh"
#include "common/logic_helper.hh"

namespace sesh {
namespace async {

// Defined just below
class runnable;

/**
 * Continuation is a suspended computation that can be resumed later.
 *
 * This class abstracts a parameter-less continuation that can be resumed at
 * any time. The continuation is automatically resumed in the destructor.
 *
 * The computation contained in a continuation object is abstracted by the
 * {@link runnable} class.
 */
class continuation {

private:

    std::shared_ptr<runnable> m_runnable;

public:

    /** Default constructor. Creates a nop continuation. */
    constexpr continuation() = default;

    /**
     * Constructs a continuation from a nullable shared pointer to a runnable.
     * The runnable, if non-null, will be run as the body of continuation.
     */
    continuation(std::shared_ptr<runnable> &&r) noexcept :
            m_runnable(std::move(r)) { }

    /** Move constructor. The argument continuation will be empty (a nop). */
    continuation(continuation &&) = default;

    /** Swaps two continuations. */
    void swap(continuation &other) noexcept {
        m_runnable.swap(other.m_runnable);
    }

    /** Move assignment operator. Swaps the two continuations. */
    continuation &operator=(continuation &&c) noexcept {
        swap(c);
        return *this;
    }

    /**
     * Resumes the suspended computation, if any. Returns after the computation
     * ends.
     *
     * The continuation is empty after the run.
     */
    inline void run() noexcept;

    /**
     * Resumes the suspended computation, if any. Returns after the computation
     * ends.
     */
    ~continuation() noexcept { run(); }

}; // class continuation

/** Swaps two continuations. */
inline void swap(continuation &l, continuation &r) noexcept {
    return l.swap(r);
}

/** Abstract computation that can be run once. */
class runnable {

private:

    virtual continuation do_run() noexcept = 0;

public:

    virtual ~runnable() = default;

    continuation run() noexcept { return do_run(); }

}; // class runnable

template<typename F, typename... A>
typename common::same_type<
        typename std::result_of<F(A...)>::type, continuation>::type
call(F &&f, A &&... a) noexcept {
    return common::invoke(std::forward<F>(f), std::forward<A>(a)...);
}

template<typename F, typename... A>
typename std::enable_if<
        std::is_void<typename std::result_of<F(A...)>::type>::value,
        continuation>::type
call(F &&f, A &&... a) noexcept {
    common::invoke(std::forward<F>(f), std::forward<A>(a)...);
    return {};
}

/**
 * Wrapper of a callable object as runnable.
 *
 * @tparam T Type of the wrapped object. Must be no-throw-callable with no
 * arguments and return void or continuation.
 */
template<typename T>
class runnable_wrapper : public runnable {

public:

    using value_type = T;

    value_type value;

    /**
     * Constructs a runnable wrapper by forwarding all the arguments to the
     * constructor of @c T.
     */
    template<
            typename... A,
            typename = typename std::enable_if<
                    std::is_constructible<T, A...>::value>::type>
    explicit runnable_wrapper(A &&... a)
            noexcept(std::is_nothrow_constructible<T, A...>::value) :
            value(std::forward<A>(a)...) { }

private:

    continuation do_run() noexcept final override { return call(value); }

}; // class runnable_wrapper

inline void continuation::run() noexcept {
    while (m_runnable)
        m_runnable = m_runnable->run().m_runnable;
}

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_continuation_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
