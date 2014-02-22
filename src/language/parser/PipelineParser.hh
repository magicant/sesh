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

#ifndef INCLUDED_language_parser_PipelineParser_hh
#define INCLUDED_language_parser_PipelineParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Variant.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Repeat.hh"
#include "language/parser/SpecificOperatorParser.hh"
#include "language/parser/TokenParser.hh"
#include "language/syntax/Pipeline.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a pipeline.
 *
 * Because the constructor of this class takes a token parser that may have
 * already started parsing, this class does not support resetting in the usual
 * way. When you reset a pipeline parser, a new token parser is created in a
 * neutral state.
 *
 * This is an abstract class that implements some part of the parser. A
 * concrete subclass must provide factory methods that create parsers used by
 * this parser.
 */
class PipelineParser : public NormalParser<std::unique_ptr<syntax::Pipeline>> {

public:

    using PipelinePointer = std::unique_ptr<syntax::Pipeline>;
    using ExitStatusType = syntax::Pipeline::ExitStatusType;
    using CommandPointer = syntax::Pipeline::CommandPointer;

    using TokenParserPointer = std::unique_ptr<TokenParser>;
    using CommandParserPointer = std::unique_ptr<Parser<CommandPointer>>;

private:

    struct ParsingFirstToken {
        TokenParserPointer tokenParser;
    }; // class ParsingFirstToken

    struct ParsingFirstCommand {
        PipelinePointer pipeline;
        CommandParserPointer commandParser;
    }; // class ParsingFirstCommand

    struct ParsingRemainingCommands {
        PipelinePointer pipeline;
        CommandParserPointer pipedCommandParser;
    }; // class ParsingRemainingCommands

    using ParseState = common::Variant<
            ParsingFirstToken, ParsingFirstCommand, ParsingRemainingCommands>;

    ParseState mState;

    class StateProcessor;

public:

    /**
     * Constructs a new pipeline parser.
     *
     * The argument token parser is used to parse every token of the first
     * command in the pipeline. The token parser does not have to be in the
     * "unstarted" state: it may already be "parsing" the first token or even
     * "finished". If the argument token parser pointer is null, {@link
     * #createTokenParser} will be called while parsing to create a parser.
     */
    explicit PipelineParser(Environment &, TokenParserPointer && = nullptr);

private:

    virtual TokenParserPointer createTokenParser() const = 0;
    virtual CommandParserPointer createCommandParser(TokenParserPointer) const
            = 0;

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class PipelineParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_PipelineParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
