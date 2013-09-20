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

#ifndef INCLUDED_language_parser_Predicate_hh
#define INCLUDED_language_parser_Predicate_hh

#include "buildconfig.h"

#include <functional>
#include "common/Char.hh"
#include "language/parser/Environment.hh"

namespace sesh {
namespace language {
namespace parser {

template<typename T>
using Predicate = std::function<bool(const Environment &, T)>;

/** Determines if the argument character is a newline (<code>'\n'</code>). */
bool isNewline(const Environment &, common::Char);

/** Determines if the argument character is a blank. */
bool isBlank(const Environment &, common::Char);

/**
 * Determines if the argument character is a standard token delimiter like a
 * blank, semicolon, parenthesis, etc.
 */
bool isTokenDelimiter(const Environment &, common::Char);

/**
 * Determines if the argument character can be included in a variable name.
 * This function may return true for a non-ASCII character.
 */
bool isVariableNameChar(const Environment &, common::Char);

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Predicate_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
