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

#ifndef INCLUDED_language_parsing_token_hh
#define INCLUDED_language_parsing_token_hh

#include "buildconfig.h"

#include "async/future.hh"
#include "common/type_tag_set.hh"
#include "common/variant.hh"
#include "language/parsing/parser.hh"
#include "language/syntax/word.hh"

namespace sesh {
namespace language {
namespace parsing {

using token_type_set = common::type_tag_set<syntax::word>;

/** Token is a syntactic component that is separated by blanks. */
using token = common::variant<syntax::word>;
// TODO support keywords, assignments, redirections, and aliases

/**
 * Parses a token. The argument type tag set specifies acceptable types of
 * tokens. If the set is empty, the parsing fails.
 *
 * This function never returns an empty word. If the resultant token is a word,
 * it has at least one component.
 */
async::future<result<token>> parse_token(token_type_set, const state &);

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_token_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
