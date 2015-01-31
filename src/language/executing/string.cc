/* Copyright (C) 2015 WATANABE Yuki
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

#include "buildconfig.h"
#include "string.hh"

#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace executing {

namespace {

using sesh::common::xstring;

} // namespace

void append(
        std::vector<word_char> &wcs,
        const xstring &s,
        bool is_literal,
        bool is_quoted) {
    wcs.reserve(wcs.size() + s.size());
    for (xstring::value_type c : s)
        wcs.push_back({c, is_literal, is_quoted});
}

} // namespace executing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
