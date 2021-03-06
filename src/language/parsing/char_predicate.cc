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

#include "buildconfig.h"
#include "char_predicate.hh"

#include <locale>
#include "common/xchar.hh"
#include "common/xstring.hh"

namespace {

using sesh::common::contains;
using sesh::common::xchar;
using sesh::common::xstring;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

bool is_blank(xchar x, const context &c) {
#if HAVE_STD__ISBLANK
    return std::isblank(x, c.locale);
#else
    (void) c.locale;
    return x == L(' ') || x == L('\t');
#endif // #if HAVE_STD__ISBLANK
}

bool is_token_char(xchar x, const context &c) {
    static const xstring delimiters = L(" \t\n;&|<>()");
    return !contains(delimiters, x) && !is_blank(x, c);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
