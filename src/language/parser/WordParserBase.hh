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

#ifndef INCLUDED_language_parser_WordParserBase_hh
#define INCLUDED_language_parser_WordParserBase_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/Predicate.hh"
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
class WordParserBase :
        public Parser<std::unique_ptr<syntax::Word>>, protected ParserBase {

protected:

    using ComponentParser = Parser<std::unique_ptr<syntax::WordComponent>>;

private:

    Predicate<common::Char> mIsDelimiter;

    std::unique_ptr<syntax::Word> mWord;

    /** May be null. */
    std::unique_ptr<ComponentParser> mCurrentComponentParser;

public:

    WordParserBase(Environment &, Predicate<common::Char> &&isDelimiter);
    WordParserBase(const WordParserBase &) = delete;
    WordParserBase(WordParserBase &&) = default;
    WordParserBase &operator=(const WordParserBase &) = delete;
    WordParserBase &operator=(WordParserBase &&) = delete;
    ~WordParserBase() override = default;

private:

    /**
     * Creates a new raw string parser that works in the same environment as
     * this.
     * @return non-null pointer to a new raw string parser.
     */
    virtual std::unique_ptr<ComponentParser> createRawStringParser(
            Predicate<common::Char> &&isDelimiter,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const = 0;

    /**
     * If mCurrentComponentParser is null, tries to create a new one. The type
     * of the new word component parser is determined by the character at the
     * environment's current iterator position. If the end of the word is
     * reached, no parser is created.
     * @throws NeedMoreSource
     */
    void createComponentParser();

public:

    /**
     * Returns the parse result. The return value is a (nullable) word pointer.
     * The null pointer means a parse error. If non-null, the word may be
     * empty, so the caller should check the emptiness for validation. The
     * environment's current iterator position is updated so that it points to
     * the character past the parsed word.
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

}; // class WordParserBase

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
