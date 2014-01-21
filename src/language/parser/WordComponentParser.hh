/* Copyright (C) 2013-2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_WordComponentParser_hh
#define INCLUDED_language_parser_WordComponentParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * This is an abstract class that implements some part of the word component
 * parser. A concrete subclass must provide factory methods that create parsers
 * used by this parser.
 *
 * This class determines the type of the word component by peeking the current
 * character and calls a factory method to create an internal parser that
 * actually parses the word component of the determined type. The result of the
 * internal parser will be that of this parser.
 *
 * This parser will fail if there is no word component at the current position.
 */
class WordComponentParser :
        public Parser<std::unique_ptr<syntax::WordComponent>> {

public:

    using ComponentPointer = std::unique_ptr<syntax::WordComponent>;
    using ParserPointer = std::unique_ptr<Parser<ComponentPointer>>;

private:

    Predicate<common::Char> mIsAcceptableChar;
    ParserPointer mActualParser;

public:

    WordComponentParser(
            Environment &, Predicate<common::Char> &&isAcceptableChar);

private:

    /**
     * Creates a new raw string parser that works in the same environment as
     * this.
     * @return non-null pointer to a new raw string parser.
     */
    virtual ParserPointer createRawStringParser(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const = 0;

    void prepareActualParser();

    void parseImpl() final override;

    common::Maybe<ComponentPointer> &result() noexcept final override;

    void resetImpl() noexcept final override;

}; // class WordComponentParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordComponentParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
