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

#ifndef INCLUDED_language_executing_string_hh
#define INCLUDED_language_executing_string_hh

#include "buildconfig.h"

#include <vector>
#include "common/xstring.hh"
#include "language/executing/word_char.hh"

namespace sesh {
namespace language {
namespace executing {

/** Appends characters from a string to word char vector. */
void append(
        std::vector<word_char> &,
        const common::xstring &,
        bool is_literal,
        bool is_quoted);

/** Converts a string into a word char vector with the specified flags. */
inline std::vector<word_char> expand(
        const common::xstring &s, bool is_literal, bool is_quoted) {
    std::vector<word_char> wcs;
    append(wcs, s, is_literal, is_quoted);
    return wcs;
}

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_string_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
