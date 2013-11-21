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

#ifndef INCLUDED_language_parser_token_hh
#define INCLUDED_language_parser_token_hh

#include "buildconfig.h"

#include "common/String.hh"
#include "language/parser/Environment.hh"

namespace sesh {
namespace language {
namespace parser {

namespace token {

extern const common::String AND;
extern const common::String AND_AND;
extern const common::String GREATER;
extern const common::String GREATER_AND;
extern const common::String GREATER_GREATER;
extern const common::String GREATER_PIPE;
extern const common::String LEFT_PARENTHESIS;
extern const common::String LESS;
extern const common::String LESS_AND;
extern const common::String LESS_GREATER;
extern const common::String LESS_LESS;
extern const common::String LESS_LESS_MINUS;
extern const common::String LESS_PIPE;
extern const common::String PIPE;
extern const common::String PIPE_PIPE;
extern const common::String RIGHT_PARENTHESIS;
extern const common::String SEMICOLON;
extern const common::String SEMICOLON_SEMICOLON;

extern const common::String CASE;
extern const common::String DO;
extern const common::String DONE;
extern const common::String ELIF;
extern const common::String ELSE;
extern const common::String ESAC;
extern const common::String EXCLAMATION;
extern const common::String FI;
extern const common::String FOR;
extern const common::String FUNCTION;
extern const common::String IF;
extern const common::String IN;
extern const common::String LEFT_BRACE;
extern const common::String LEFT_BRACKET_BRACKET;
extern const common::String RIGHT_BRACE;
extern const common::String RIGHT_BRACKET_BRACKET;
extern const common::String SELECT;
extern const common::String THEN;
extern const common::String UNTIL;
extern const common::String WHILE;

} // namespace token

/**
 * Checks if there is a symbol at the current iterator position of the
 * environment.
 *
 * Returns the symbol as a string. If no symbol was found, an empty string is
 * returned.
 *
 * Line continuations are removed if encountered while parsing. The iterator
 * position of the environment is never changed.
 *
 * If more source is needed to finish parsing the symbol, this function
 * throws NeedMoreSource. In this case, the caller should set the EOF flag
 * or append to the source and then call this function again.
 *
 * @throws NeedMoreSource (see above)
 */
const common::String &peekSymbol(Environment &);

/**
 * Parses a symbol from the current iterator position of the environment.
 *
 * Returns the parsed symbol as a string. If no symbol was found, an empty
 * string is returned.
 *
 * The iterator position is incremented past the parsed symbol if (and only if)
 * a valid symbol was parsed.
 *
 * Line continuations are removed if encountered while parsing.
 *
 * If more source is needed to finish parsing the symbol, this function
 * throws NeedMoreSource. In this case, the caller should set the EOF flag
 * or append to the source and then call this function again.
 *
 * @throws NeedMoreSource (see above)
 */
const common::String &parseSymbol(Environment &);

/**
 * Checks if there is a keyword (reserved word) token at the environment's
 * current position.
 *
 * Returns the keyword as a string. If no keyword was found, an empty string is
 * returned.
 *
 * Line continuations are removed if encountered while parsing. The iterator
 * position of the environment is never changed.
 *
 * If more source is needed to finish parsing the keyword, this function throws
 * NeedMoreSource. In this case, the caller should set the EOF flag or append
 * to the source and then call this function again.
 *
 * @throws NeedMoreSource (see above)
 */
const common::String &peekKeyword(Environment &);

/**
 * Parses a keyword (reserved word) token from the current iterator position of
 * the environment.
 *
 * Returns the parsed keyword as a string. If no keyword was found, an empty
 * string is returned.
 *
 * The iterator position is incremented past the parsed keyword if (and only
 * if) a valid keyword was parsed.
 *
 * Line continuations are removed if encountered while parsing.
 *
 * If more source is needed to finish parsing the keyword, this function throws
 * NeedMoreSource. In this case, the caller should set the EOF flag or append
 * to the source and then call this function again.
 *
 * @throws NeedMoreSource (see above)
 */
const common::String &parseKeyword(Environment &);

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_token_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
