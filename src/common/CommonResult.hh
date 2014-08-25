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

#ifndef INCLUDED_common_CommonResult_hh
#define INCLUDED_common_CommonResult_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>

namespace sesh {
namespace common {

namespace common_result_impl {

inline void ignore(...);

template<typename Function, typename... Argument>
inline auto isCallable(Function &&f, Argument &&... a)
    -> decltype(
        ignore((std::forward<Function>(f)(std::forward<Argument>(a)), 0)...),
        std::true_type());

inline std::false_type isCallable(...);

template<typename, typename>
class SameType { };

template<typename T>
class SameType<T, T> {
public:
    using type = T;
};

template<bool IS_CALLABLE, typename Function, typename... Argument>
class CommonResultImpl;

template<typename Function, typename... Argument>
class CommonResultImpl<false, Function, Argument...> { };

template<typename Function>
class CommonResultImpl<true, Function> { };

template<typename Function, typename Argument>
class CommonResultImpl<true, Function, Argument> {
public:
    using type = decltype(std::declval<Function>()(std::declval<Argument>()));
};

template<typename Function, typename A1, typename A2, typename... AN>
class CommonResultImpl<true, Function, A1, A2, AN...> :
        public SameType<
                typename CommonResultImpl<true, Function, A1>::type,
                typename CommonResultImpl<true, Function, A2, AN...>::type> {
};

} // namespace common_result_impl

/**
 * A specialization of this class template will have the "type" member type
 * alias which is the return type of function calls with the parameter function
 * and argument types.
 *
 * If the function call returns the same return type for all given argument
 * types, the common result class template has the member type alias "type"
 * which is the return type. If the return type differs, or the function is not
 * callable with some of the arguments, or no argument types are given, then
 * the member type alias is not defined.
 *
 * @tparam Function A function object type.
 * @tparam Argument Argument types that are passed to the function.
 */
template<typename Function, typename... Argument>
class CommonResult :
        public common_result_impl::CommonResultImpl<
                decltype(common_result_impl::isCallable(
                        std::declval<Function>(),
                        std::declval<Argument>()...))::value,
                Function,
                Argument...> { };

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_CommonResult_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
