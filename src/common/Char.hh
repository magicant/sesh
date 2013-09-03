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

#ifndef INCLUDED_common_Char_hh
#define INCLUDED_common_Char_hh

namespace sesh {
namespace common {

/**
 * The character type that is used throughout the program (except when calling
 * an OS API function).
 */
using Char = wchar_t;

/**
 * This macro converts a character or string literal of the char type to an
 * equivalent literal of the Char type.
 */
#define L(x) L##x

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_Char_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
