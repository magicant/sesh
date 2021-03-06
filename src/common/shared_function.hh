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

#ifndef INCLUDED_common_shared_function_hh
#define INCLUDED_common_shared_function_hh

#include "buildconfig.h"

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include "common/direct_initialize.hh"

namespace sesh {
namespace common {

/**
 * A copyable wrapper for non-copyable function.
 *
 * The std::function class template requires the contained callable object to
 * be copy-constructible so that the contained object can be copied when the
 * containing std::function object is copied. However, we sometimes need to put
 * a non-copyable object in a std::function, especially when the contained
 * object contains a unique pointer. This shared function class template can be
 * used to wrap a non-copyable object and pretend to be copyable.
 *
 * The function object contained in a shared function instance is never copied
 * once it is constructed. When the shared function is copied, a shared pointer
 * to the function object, instead of the function itself, is copied.
 *
 * @tparam F type of the wrapped function object.
 */
template<typename F>
class shared_function {

private:

    std::shared_ptr<F> m_function;

public:

    /**
     * Constructs a shared function that wraps a new instance of F.
     *
     * The wrapped F object is constructed by std::make_shared.
     *
     * @param arg arguments that are passed to std::make_shared.
     */
    template<typename... Arg>
    explicit shared_function(direct_initialize, Arg &&... arg) :
            m_function(std::make_shared<F>(std::forward<Arg>(arg)...)) { }

    /**
     * Constructs a shared function that wraps a new instance of F.
     *
     * The wrapped F object is constructed by std::make_shared.
     *
     * @param arg arguments that are passed to std::make_shared.
     */
    template<typename... Arg>
    static shared_function create(Arg &&... arg) {
        return shared_function(direct_initialize(), std::forward<Arg>(arg)...);
    }

    /**
     * Constructs a shared function that wraps a new instance of F.
     *
     * The wrapped F object is constructed by std::allocate_shared.
     *
     * @param arg arguments that are passed to std::allocate_shared.
     */
    template<typename... Arg>
    explicit shared_function(std::allocator_arg_t, Arg &&... arg) :
            m_function(std::allocate_shared<F>(std::forward<Arg>(arg)...)) { }

    /**
     * Constructs a shared function from an existing non-null shared pointer.
     */
    shared_function(const std::shared_ptr<F> &f) noexcept : m_function(f) {
        assert(m_function != nullptr);
    }

    /**
     * Constructs a shared function from an existing non-null shared pointer.
     */
    shared_function(std::shared_ptr<F> &&f) noexcept :
            m_function(std::move(f)) {
        assert(m_function != nullptr);
    }

    /** Returns a reference to the wrapped function object. */
    F &get() const { return *m_function; }

    /**
     * Calls the wrapped object as a function.
     * @tparam Arg types of function arguments.
     * @param arg arguments that are passed to the function.
     */
    template<typename... Arg>
    auto operator()(Arg &&... arg) const
            -> typename std::result_of<F(Arg...)>::type {
        return get()(std::forward<Arg>(arg)...);
    }

}; // template<typename F> class shared_function

/**
 * Constructs a new shared function from the argument function object.
 *
 * A new instance of G which is to be shared is copy- or move-constructed from
 * the argument.
 */
template<typename F, typename G = const typename std::decay<F>::type>
shared_function<G> make_shared_function(F &&f) {
    return shared_function<G>(direct_initialize(), std::forward<F>(f));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_shared_function_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
