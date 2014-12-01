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

#ifndef INCLUDED_common_xsstream_hh
#define INCLUDED_common_xsstream_hh

#include "buildconfig.h"

#include <sstream>
#include "common/xchar.hh"

namespace sesh {
namespace common {

using xstringbuf = std::basic_stringbuf<xchar>;
using xistringstream = std::basic_istringstream<xchar>;
using xostringstream = std::basic_ostringstream<xchar>;
using xstringstream = std::basic_stringstream<xchar>;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_xsstream_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
