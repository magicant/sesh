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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_parser_NeedMoreSource_hh
#define INCLUDED_language_parser_NeedMoreSource_hh

#include <exception>

namespace sesh {
namespace language {
namespace parser {

/**
 * A parser throws this exception when it needs further source input to
 * complete parsing.
 *
 * If this exception is thrown by the parser, the parser user should append
 * more input to the source buffer or set the EOF flag in the parser
 * environment and then restart the parser. (The source buffer can only be
 * appended to (but not otherwise modified) because the parser may be holding
 * an iterator to the middle of the existing buffer.)
 *
 * As required by POSIX (2013 XCU-4. "sh"), the source code reader must not
 * read input more than necessary to parse the currently parsed command.
 * Therefore, the source code is passed to the parser line by line, which means
 * the parser needs a way to request a next line of source when it finds the
 * source is insufficient to compose a single command. Hence, this exception.
 */
class NeedMoreSource : public std::exception {
};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_NeedMoreSource_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
