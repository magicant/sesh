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

#ifndef INCLUDED_language_parsing_line_continuation_hh
#define INCLUDED_language_parsing_line_continuation_hh

#include "buildconfig.h"

#include <tuple>
#include "common/empty.hh"
#include "common/xchar.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * Skips a line continuation and returns a tuple of the two characters that
 * make up the line continuation.
 *
 * This parser fails if there is no line continuation at the current position.
 *
 * This parser never returns any reports even on failure.
 */
extern parser<std::tuple<common::xchar, common::xchar>> skip_line_continuation;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_line_continuation_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
