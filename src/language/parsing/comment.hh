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

#ifndef INCLUDED_language_parsing_comment_hh
#define INCLUDED_language_parsing_comment_hh

#include "buildconfig.h"

#include "common/xstring.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * Skips a hash sign <code>#</code> and following characters until the next
 * newline (or end of input). Ignores any line continuations.
 *
 * This parser function doesn't report any error even on failure.
 *
 * The resultant string is the body of the comment (not including the beginning
 * hash sign).
 */
extern parser<common::xstring> skip_comment;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_comment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
