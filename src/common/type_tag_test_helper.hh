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

#ifndef INCLUDED_common_type_tag_test_helper_hh
#define INCLUDED_common_type_tag_test_helper_hh

#include "buildconfig.h"

#include <ostream>
#include <typeinfo>
#include "common/type_tag.hh"

namespace sesh {
namespace common {

namespace type_tag_impl {

template<typename Char, typename Traits>
struct printer {

    std::basic_ostream<Char, Traits> &os;

    template<typename U>
    std::basic_ostream<Char, Traits> &operator()(type_tag<U>) {
        return os << typeid(U).name();
    }

}; // template<typename Char, typename Traits> struct printer

} // namespace type_tag_impl

template<typename Char, typename Traits, typename... T>
inline std::basic_ostream<Char, Traits> &operator<<(
        std::basic_ostream<Char, Traits> &os, type_tag<T...> t) {
    return t.apply(type_tag_impl::printer<Char, Traits>{os});
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_type_tag_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
