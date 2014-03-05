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

#ifndef INCLUDED_language_parser_TokenParser_hh
#define INCLUDED_language_parser_TokenParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/EnumSet.hh"
#include "language/parser/BlankAndCommentParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Token.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a token in a command. The result is a (normal) word, keyword,
 * assignment, or redirection. This parser never returns an empty word; the
 * parser would rather fail when there is no valid word.
 *
 * The parsed word may optionally preceded by blank characters and a comment,
 * which are skipped before the token is parsed.
 *
 * This is an abstract class that switches word, assignment, and redirection
 * parsers. A concrete subclass must provide factory methods that create those
 * parsers.
 */
class TokenParser : public Parser<Token> {

protected:

    using AssignmentPointer = std::unique_ptr<syntax::Assignment>;
    using AssignmentParserPointer = std::unique_ptr<Parser<AssignmentPointer>>;

    using WordPointer = std::unique_ptr<syntax::Word>;
    using WordParserPointer = std::unique_ptr<Parser<WordPointer>>;

private:

    BlankAndCommentParser mBlankAndCommentParser;

    common::EnumSet<TokenType> mAcceptableTokenTypes;

    AssignmentParserPointer mAssignmentParser;
    WordParserPointer mWordParser;

    Token mResultToken;

public:

    TokenParser(Environment &, common::EnumSet<TokenType> acceptableTokenTypes)
            noexcept;

private:

    virtual AssignmentParserPointer createAssignmentParser() const = 0;
    virtual WordParserPointer createWordParser(
            Predicate<common::Char> &&isAcceptableChar) const = 0;

    bool maybeParseAssignment();
    WordPointer &parseWord();
    bool maybeParseKeyword();
    bool maybeParseWord();

    void parseImpl() final override;

    void resetImpl() noexcept final override;

public:

    using Parser::reset;

    void reset(common::EnumSet<TokenType> acceptableTokenTypes) noexcept;

}; // class TokenParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_TokenParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
