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

#ifndef INCLUDED_language_parser_CommandParser_hh
#define INCLUDED_language_parser_CommandParser_hh

#include "buildconfig.h"

#include <memory>
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/TokenParser.hh"
#include "language/syntax/Command.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a command.
 *
 * This parser never fails unless the token parser fails to parse the first
 * token.
 *
 * Because of the fact that the constructor of this class takes a token parser
 * that may have already started parsing, this class does not support resetting
 * in the usual way. When you reset a command parser, a new token parser is
 * created in a neutral state.
 *
 * This is an abstract class that implements some part of the parser. A
 * concrete subclass must provide factory methods that create parsers used by
 * this parser.
 */
class CommandParser : public Parser<std::unique_ptr<syntax::Command>> {

public:

    using CommandPointer = std::unique_ptr<syntax::Command>;

    using TokenParserPointer = std::unique_ptr<TokenParser>;
    using CommandParserPointer = std::unique_ptr<Parser<CommandPointer>>;

private:

    TokenParserPointer mTokenParser;
    CommandParserPointer mActualParser;

public:

    /**
     * Constructs a new command parser.
     *
     * The argument token parser is used to parse every token of the command.
     * The token parser does not have to be in the "unstarted" state: it may
     * already be "parsing" the first token or even "finished". If the argument
     * token parser pointer is null, {@link #createTokenParser} will be called
     * while parsing to create a parser.
     */
    explicit CommandParser(Environment &e, TokenParserPointer = nullptr)
            noexcept;

private:

    /**
     * Creates a new token parser that parses tokens in the command. This
     * function is called when a token parser was not given in the constructor
     * or when the command parser was reset.
     */
    virtual TokenParserPointer createTokenParser() const = 0;

    virtual CommandParserPointer createSimpleCommandParser(TokenParserPointer)
            const = 0;

    void prepareActualParser();

    void parseImpl() final override;

    common::Maybe<CommandPointer> &result() noexcept final override;

    void resetImpl() noexcept final override;

}; // class CommandParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CommandParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
