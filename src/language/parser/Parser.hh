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

#ifndef INCLUDED_language_parser_Parser_hh
#define INCLUDED_language_parser_Parser_hh

#include "buildconfig.h"

namespace sesh {
namespace language {
namespace parser {

/**
 * Abstract base class for parser classes.
 *
 * @tparam Result The type of an object returned as a result. Typically this
 * type is a unique pointer to an actual result object.
 */
template<typename Result>
class Parser {

public:

    virtual ~Parser() = default;

    /**
     * Parses (part of) the source and returns the result. Typically the
     * <code>Result</code> type is a pointer to something and a null pointer
     * means a parse error. The precise meaning of the result is defined by
     * each subclass.
     *
     * If this parser has an associated {@link Environment}, its current
     * iterator position is updated so that it points to the character just
     * past the parsed part of the source.
     *
     * If this function returns a result without throwing, the internal state
     * of this parser is no longer valid and this function must never be called
     * again.
     *
     * If more source is needed to finish parsing, this function throws
     * NeedMoreSource. In this case, the caller should update the source and
     * then call this function again. (Normally, the caller should either set
     * the EOF flag or append to the source in the environment.)
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    virtual Result parse() = 0;
    // TODO what if alias substitution happened?

}; // template class Parser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Parser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
