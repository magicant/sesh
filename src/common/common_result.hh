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

#ifndef INCLUDED_common_common_result_hh
#define INCLUDED_common_common_result_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "common/empty.hh"
#include "common/function_helper.hh"
#include "common/logic_helper.hh"

namespace sesh {
namespace common {

namespace common_result_impl {

template<typename Function, typename... Argument>
class common_result_impl :
        public same_type<result_of_t<Function(Argument)>...> { };

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
class common_result :
        public std::conditional<
                for_all<is_callable<Function(Argument)>::value...>::value,
                common_result_impl::common_result_impl<Function, Argument...>,
                empty
        >::type { };

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_common_result_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
