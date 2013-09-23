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

#ifndef INCLUDED_language_parser_WordParserImpl_hh
#define INCLUDED_language_parser_WordParserImpl_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "common/Char.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"

namespace sesh {
namespace language {

namespace syntax {

class Word;

} // namespace syntax

namespace parser {

class WordComponentParser;

/**
 * Word parser.
 *
 * @param Types A placeholder type that specify word component parser types
 * that are used by the word parser.
 */
template<typename Types>
class WordParserImpl : public Parser {

private:

    Predicate<common::Char> mIsDelimiter;

    std::unique_ptr<syntax::Word> mWord;

    /** May be null. */
    std::unique_ptr<WordComponentParser> mCurrentComponentParser;

public:

    WordParserImpl(Environment &, Predicate<common::Char> &&isDelimiter);
    WordParserImpl(const WordParserImpl &) = delete;
    WordParserImpl(WordParserImpl &&) = default;
    WordParserImpl &operator=(const WordParserImpl &) = delete;
    WordParserImpl &operator=(WordParserImpl &&) = delete;
    ~WordParserImpl() = default;

private:

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
     * @throws NeedMoreSource (see above)
     * @throws std::logic_error when the state of the parser is invalid.
     */
    std::unique_ptr<syntax::Word> parse();

};

} // namespace parser

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParserImpl_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
