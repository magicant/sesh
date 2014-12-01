/* Copyright (C) 2013 WATANABE Yuki
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

#ifndef INCLUDED_common_xstring_hh
#define INCLUDED_common_xstring_hh

#include "buildconfig.h"

#include <string>
#include "common/xchar.hh"

namespace sesh {
namespace common {

/**
 * The string type that is used throughout the program (except when calling an
 * OS API function).
 */
using xstring = std::basic_string<xchar>;

/** The character traits type of xstring. */
using xchar_traits = xstring::traits_type;

/** Checks if the given string contains the given character. */
template<typename CharT, typename Traits, typename Allocator>
bool contains(
        const std::basic_string<CharT, Traits, Allocator> &s,
        typename std::basic_string<CharT, Traits, Allocator>::value_type c,
        typename std::basic_string<CharT, Traits, Allocator>::size_type pos =
                0) {
    return s.find(c, pos) != std::basic_string<CharT, Traits, Allocator>::npos;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_xstring_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
