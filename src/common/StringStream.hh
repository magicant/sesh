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

#ifndef INCLUDED_common_StringStream_hh
#define INCLUDED_common_StringStream_hh

#include "buildconfig.h"

#include <sstream>
#include "common/Char.hh"

namespace sesh {
namespace common {

using StringBuffer = std::basic_stringbuf<Char>;
using InputStringStream = std::basic_istringstream<Char>;
using OutputStringStream = std::basic_ostringstream<Char>;
using StringStream = std::basic_stringstream<Char>;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_StringStream_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
