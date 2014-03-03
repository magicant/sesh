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

#ifndef INCLUDED_language_parser_SimpleCommandParser_hh
#define INCLUDED_language_parser_SimpleCommandParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/EnumSet.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParser.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/SimpleCommand.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a simple command.
 *
 * Because of the fact that the constructor of this class takes a token parser
 * that may have already started parsing, this class does not support resetting
 * in the usual way. When you reset the simple command parser, the internal
 * token parser is reset to a "default" state.
 */
class SimpleCommandParser : public Parser<std::unique_ptr<syntax::Command>> {

public:

    using CommandPointer = std::unique_ptr<syntax::Command>;
    using SimpleCommandPointer = std::unique_ptr<syntax::SimpleCommand>;

    using TokenParserPointer = std::unique_ptr<TokenParser>;

private:

    TokenParserPointer mTokenParser;

    SimpleCommandPointer mCommand;
    CommandPointer mResultCommand;

public:

    /**
     * Constructs a new simple command parser.
     *
     * The argument token parser is used to parse every token of the simple
     * command. The token parser does not have to be in the "unstarted" state:
     * it may already be "parsing" the first token or even "finished". If the
     * token parser returns a keyword as the first token, this parser reports a
     * syntax error (but the parser still succeeds).
     */
    SimpleCommandParser(Environment &, TokenParserPointer &&) noexcept;

private:

    using TokenTypeSet = common::EnumSet<TokenType>;

    class TokenParserResultAcceptor;

    TokenTypeSet nextTokenTypes() const;

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class SimpleCommandParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SimpleCommandParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
