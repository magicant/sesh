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

#ifndef INCLUDED_language_parser_WordParser_hh
#define INCLUDED_language_parser_WordParser_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Word parser. This is an abstract class that implements most part of the
 * parser. A concrete subclass must provide factory methods that create parsers
 * used by this parser.
 */
class WordParser :
        public Parser<std::unique_ptr<syntax::Word>>, protected ParserBase {

public:

    using ComponentPointer = std::unique_ptr<syntax::WordComponent>;
    using ComponentParserPointer = std::unique_ptr<Parser<ComponentPointer>>;

    /**
     * A function of this type is called to create word component parsers while
     * parsing the word. The function is called each time the word parser
     * starts parsing a component of the word.
     *
     * The function must return a new component parser that should be used to
     * parse the component starting from the current position of the
     * environment. If there is no valid component at the current position,
     * either the component parser pointer returned by the function or the
     * component pointer returned by the parser should be null.
     *
     * The function may peek a character at the current position to determine
     * the type of the component parser it creates. The function may remove
     * line continuations from the token, but may not modify the environment
     * otherwise.
     */
    using ComponentParserCreator =
            std::function<ComponentParserPointer(Environment &)>;

private:

    ComponentParserCreator mCreateComponentParser;

    std::unique_ptr<syntax::Word> mWord;

    /** May be null. */
    ComponentParserPointer mComponentParser;

public:

    WordParser(Environment &, ComponentParserCreator &&);
    WordParser(const WordParser &) = delete;
    WordParser(WordParser &&) = default;
    WordParser &operator=(const WordParser &) = delete;
    WordParser &operator=(WordParser &&) = delete;
    ~WordParser() override = default;

private:

    bool parseComponent();

public:

    /**
     * Returns the parse result. The return value is a non-null word pointer.
     * The word may be empty, so the caller should check the emptiness for
     * validation. The environment's current iterator position is updated so
     * that it points to the character past the parsed word.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to finish parsing the word, this function
     * throws NeedMoreSource. In this case, the caller should set the EOF flag
     * or append to the source and then call this function again.
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    std::unique_ptr<syntax::Word> parse() final override;

}; // class WordParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
