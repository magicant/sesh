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

#ifndef INCLUDED_language_parser_WordParser_hh
#define INCLUDED_language_parser_WordParser_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "common/Char.hh"
#include "language/parser/Converter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Repeat.hh"
#include "language/parser/WordComponentParserImpl.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a series of word components into a word.
 *
 * This parser always succeeds and returns a pointer to a new word that may
 * contain any number of (possibly zero) word components. It's the caller's
 * responsibility to check the validity of the word, especially when the caller
 * is expecting a non-empty word.
 */
class WordParser : public Converter<
        Repeat<WordComponentParserImpl>, std::unique_ptr<syntax::Word>> {

public:

    /**
     * Constructs a word parser.
     *
     * @param e environment
     * @param isAcceptableChar a predicate that determines if a character
     * should be included in the result word or the word should be delimited.
     * The predicate should not accept special characters such as tilde,
     * dollar, and quotation marks.
     */
    explicit WordParser(
            Environment &e, Predicate<common::Char> &&isAcceptableChar);

private:

    using ComponentPointer = syntax::Word::ComponentPointer;

    void convert(std::vector<ComponentPointer> &&components);

}; // class WordParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
