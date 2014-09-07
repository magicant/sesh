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

#ifndef INCLUDED_common_function_helper_hh
#define INCLUDED_common_function_helper_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "common/empty.hh"

namespace sesh {
namespace common {

namespace function_helper_impl {

// normal function
template<typename Function, typename... Argument>
auto invoke(Function &&f, Argument &&... a)
    -> decltype(std::forward<Function>(f)(std::forward<Argument>(a)...));

// data member pointer with reference
template<typename M, typename B, typename D>
auto invoke(M B::*&& p, D &&d)
    -> decltype(std::forward<D>(d).*std::forward<M B::*>(p));

// data member pointer with pointer
template<typename M, typename D>
auto invoke(M&& m, D &&d)
    -> decltype((*std::forward<D>(d)).*std::forward<M>(m));

// member function pointer with reference
template<typename F, typename B, typename D, typename... Arg>
auto invoke(F B::*&& p, D &&d, Arg &&... arg)
    -> decltype((std::forward<D>(d).*std::forward<F B::*>(p))(
                std::forward<Arg>(arg)...));

// member function pointer with pointer
template<typename F, typename D, typename... Arg>
auto invoke(F&& f, D &&d, Arg &&... arg)
    -> decltype(((*std::forward<D>(d)).*std::forward<F>(f))(
                std::forward<Arg>(arg)...));

template<typename Callable, typename... Argument>
auto is_callable(Callable &&c, Argument &&... a)
    -> decltype(
            invoke(std::forward<Callable>(c), std::forward<Argument>(a)...),
            std::true_type());

std::false_type is_callable(...);

template<typename Callable, typename... Argument>
class result_of {
public:
    using type = decltype(invoke(
            std::declval<Callable>(), std::declval<Argument>()...));
}; // template<typename Callable, typename... Argument> class result_of

} // namespace function_helper_impl

template<typename>
class is_callable;

/**
 * If a function call specified by the template parameters is well-typed, this
 * class is a subclass of std::true_type. Otherwise, this is a subclass of
 * std::false_type.
 *
 * @tparam Callable a callable type that is called.
 * @tparam Argument types of arguments that are passed to the callable.
 */
template<typename Callable, typename... Argument>
class is_callable<Callable(Argument...)> :
        public decltype(function_helper_impl::is_callable(
                std::declval<Callable>(), std::declval<Argument>()...)) { };

template<typename>
class result_of;

/**
 * If a function call specified by the template parameters is well-typed, this
 * class will have a "type" member type alias which is the return type of the
 * call. Otherwise, this is an empty class.
 *
 * @tparam Callable a callable type that is called.
 * @tparam Argument types of arguments that are passed to the callable.
 */
template<typename Callable, typename... Argument>
class result_of<Callable(Argument...)> :
        public std::conditional<
                is_callable<Callable(Argument...)>::value,
                function_helper_impl::result_of<Callable, Argument...>,
                empty
        >::type { };

template<typename T>
using result_of_t = typename result_of<T>::type;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_function_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
