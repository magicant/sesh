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

#ifndef INCLUDED_language_parser_WordComponentParserBase_hh
#define INCLUDED_language_parser_WordComponentParserBase_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/Predicate.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Word component parser. This is an abstract class that implements some part
 * of the parser. A concrete subclass must provide factory methods that create
 * parsers used by this parser.
 */
class WordComponentParserBase :
        public Parser<std::unique_ptr<syntax::WordComponent>>,
        protected ParserBase {

public:

    using ComponentPointer = std::unique_ptr<syntax::WordComponent>;
    using ParserPointer = std::unique_ptr<Parser<ComponentPointer>>;

private:

    Predicate<common::Char> mIsDelimiter;
    ParserPointer mActualParser;

public:

    WordComponentParserBase(
            Environment &, Predicate<common::Char> &&isDelimiter);
    WordComponentParserBase(WordComponentParserBase &&) = default;
    ~WordComponentParserBase() override = default;

private:

    void prepareActualParser();

    /**
     * Creates a new raw string parser that works in the same environment as
     * this.
     * @return non-null pointer to a new raw string parser.
     */
    virtual ParserPointer createRawStringParser(
            Predicate<common::Char> &&isDelimiter,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const = 0;

public:

    /**
     * Returns the parse result. The return value is a (nullable) word
     * component pointer. A null result means either that there is no word
     * component at the current position or that there was a parse error while
     * parsing the component. The environment's current iterator position is
     * updated so that it points to the character past the parsed word.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to finish parsing the component, this function
     * throws NeedMoreSource. In this case, the caller should set the EOF flag
     * or append to the source and then call this function again.
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    ComponentPointer parse() final override;

}; // class WordComponentParserBase

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordComponentParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
