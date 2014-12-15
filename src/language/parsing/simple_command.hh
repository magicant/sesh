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

#ifndef INCLUDED_language_parsing_simple_command_hh
#define INCLUDED_language_parsing_simple_command_hh

#include "buildconfig.h"

#include "common/variant.hh"
#include "language/parsing/parser.hh"
#include "language/syntax/simple_command.hh"

namespace sesh {
namespace language {
namespace parsing {

// TODO support keywords and aliases
using simple_command_parse = common::variant<syntax::simple_command>;

/**
 * Parses a simple command by parsing blank-separated tokens. This parser skips
 * any whitespaces after the last token.
 *
 * If the first token is a keyword or alias, that is returned instead of a
 * simple command. If there is no token, the parser fails with an error.
 */
extern parser<simple_command_parse> parse_simple_command;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_simple_command_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
