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

#ifndef INCLUDED_language_parser_AssignedWordParser_hh
#define INCLUDED_language_parser_AssignedWordParser_hh

#include "buildconfig.h"

#include <memory>
#include "language/parser/CharParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses the right-hand-side of an assignment.
 *
 * The target is very similar to normal word, but different in that tilde
 * expansions are recognized not only at the beginning of the word but also in
 * the middle after a separating colon.
 */
class AssignedWordParser : public Parser<std::unique_ptr<syntax::Word>> {

public:

    using WordPointer = std::unique_ptr<syntax::Word>;
    using WordParserPointer = std::unique_ptr<Parser<WordPointer>>;

private:

    enum class State { WORD, COLON, };

    State mState;
    WordParserPointer mWordParser;
    CharParser mColonParser;

    std::unique_ptr<syntax::Word> mResultWord;

public:

    /**
     * Constructs a new assigned word parser.
     * @param wordParser non-null pointer to a word parser that is used to
     * parse most part of the assigned word. If this parser's state is already
     * "parsing" or "finished", the result will be used as the beginning part
     * of the resultant assigned word. This parser will be used repeatedly to
     * parse each colon-separated part of the assigned word. This parser must
     * stop parsing when an unquoted colon is found.
     */
    AssignedWordParser(WordParserPointer &&wordParser);

private:

    bool parseWord();
    bool parseColon();
    bool parseComponent();

    void parseImpl() override;

    void resetImpl() noexcept override;

}; // class AssignedWordParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignedWordParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
