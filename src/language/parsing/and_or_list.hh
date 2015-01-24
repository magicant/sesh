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

#ifndef INCLUDED_language_parsing_and_or_list_hh
#define INCLUDED_language_parsing_and_or_list_hh

#include "buildconfig.h"

#include "common/variant.hh"
#include "language/parsing/parser.hh"
#include "language/syntax/and_or_list.hh"

namespace sesh {
namespace language {
namespace parsing {

// TODO support keywords and aliases
using and_or_list_parse = common::variant<syntax::and_or_list>;

/**
 * Parses an and-or list. This parser skips any whitespaces after the last
 * token.
 *
 * If the first token is an alias or a keyword that does not start an and-or
 * list, that is returned instead of an and-or list. If there is no token, the
 * parser fails with an error.
 */
extern parser<and_or_list_parse> parse_and_or_list;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_and_or_list_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
