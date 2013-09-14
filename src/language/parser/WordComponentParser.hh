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

#ifndef INCLUDED_language_parser_WordComponentParser_hh
#define INCLUDED_language_parser_WordComponentParser_hh

#include <memory>

namespace sesh {
namespace language {

namespace syntax {

class WordComponent;

} // namespace syntax

namespace parser {

/** Abstract base class for word component parsers. */
class WordComponentParser {

public:

    WordComponentParser() = default;
    WordComponentParser(const WordComponentParser &) = default;
    WordComponentParser(WordComponentParser &&) = default;
    WordComponentParser &operator=(const WordComponentParser &) = delete;
    WordComponentParser &operator=(WordComponentParser &&) = delete;
    virtual ~WordComponentParser() = default;

    /**
     * Returns the parse result. The return value is a (nullable) word
     * component pointer. A null pointer means a parse error. If this parser
     * has an associated environment, its current iterator position is updated
     * so that it points to the character just past the parsed word.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to finish parsing the word component, this
     * function throws NeedMoreSource. In this case, the caller should set the
     * EOF flag or append to the source and then call this function again.
     *
     * @throws NeedMoreSource (see above)
     * @throws std::logic_error when the state of the parser is invalid.
     */
    virtual std::unique_ptr<syntax::WordComponent> parse() = 0;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordComponentParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
