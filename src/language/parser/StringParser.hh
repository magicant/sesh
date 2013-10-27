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

#ifndef INCLUDED_language_parser_StringParser_hh
#define INCLUDED_language_parser_StringParser_hh

#include "buildconfig.h"

#include <functional>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Skipper.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A simple parser that extracts a string from the initial position up to a
 * delimiter.
 */
class StringParser : public Parser {

private:

    Iterator mBegin;
    Skipper mSkipper;

public:

    /**
     * Constructs a string parser that works in the specified environment. The
     * string is parsed from the environment's current iterator position up to
     * (but not including) the first encountered delimiter (or the end of
     * source if no delimiter found). The argument predicate determines if a
     * character is a delimiter.
     */
    StringParser(
            Environment &,
            Predicate<common::Char> &&isDelimiter,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

    StringParser(const StringParser &) = default;
    StringParser(StringParser &&) = default;
    StringParser &operator=(const StringParser &) = delete;
    StringParser &operator=(StringParser &&) = delete;
    ~StringParser() = default;

public:

    /**
     * Returns the result string. The current iterator position of the
     * Environment is updated so that it points to the character just past the
     * string. The string is delimited at the first delimiter character that
     * appears not before the current position.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to find the delimiter, this function throws
     * NeedMoreSource. In this case, the caller should set the EOF flag or
     * append to the source and then call this function again.
     *
     * @throws NeedMoreSource (see above)
     */
    common::String parse();

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_StringParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
