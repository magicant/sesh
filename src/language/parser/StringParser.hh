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

#ifndef INCLUDED_language_parser_StringParser_hh
#define INCLUDED_language_parser_StringParser_hh

#include "buildconfig.h"

#include "common/String.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/Repeat.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A simple parser that extracts a string from the initial position up to a
 * delimiter (a character that is not accepted by the given predicate).
 */
using StringParser = Repeat<CharParser, common::String>;

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_StringParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
