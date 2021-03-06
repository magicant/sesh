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

#ifndef INCLUDED_common_logic_helper_hh
#define INCLUDED_common_logic_helper_hh

#include "buildconfig.h"

#include <type_traits>

namespace sesh {
namespace common {

/**
 * This class template is defined to be a subclass of either std::true_type or
 * std::false_type. The value is true iff all the template parameters are true.
 */
template<bool...>
class for_all;

template<>
class for_all<> : public std::true_type { };

template<bool... b>
class for_all<true, b...> : public for_all<b...> { };

template<bool... b>
class for_all<false, b...> : public std::false_type { };

/**
 * This class template is defined to be a subclass of either std::true_type or
 * std::false_type. The value is false iff all the template parameters are
 * false.
 */
template<bool...>
class for_any;

template<>
class for_any<> : public std::false_type { };

template<bool... b>
class for_any<true, b...> : public std::true_type { };

template<bool... b>
class for_any<false, b...> : public for_any<b...> { };

/**
 * This class template will have the "type" member type alias iff all the
 * template parameters are the same type. The alias is not declared if no
 * template parameter is given.
 */
template<typename...>
class same_type { };

template<typename T>
class same_type<T> {
public:
    using type = T;
};

template<typename Head, typename... Tail>
class same_type<Head, Head, Tail...> : public same_type<Head, Tail...> { };

/**
 * Defined to be (a subclass of) std::true_type or std::false_type depending on
 * the template parameter types. The Boolean will be true if and only if
 * @c T is the same type as one (or more) of <code>U</code>s.
 */
template<typename T, typename... U>
class is_any_of : public for_any<std::is_same<T, U>::value...> { };

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_logic_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
