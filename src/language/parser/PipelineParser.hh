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

#ifndef INCLUDED_language_parser_PipelineParser_hh
#define INCLUDED_language_parser_PipelineParser_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "common/Maybe.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/Pipeline.hh"

namespace sesh {
namespace language {
namespace parser {

/** Pipeline parser. */
class PipelineParser final :
        public Parser<std::unique_ptr<syntax::Pipeline>>,
        protected ParserBase {

public:

    using CommandParser = Parser<std::unique_ptr<syntax::Command>>;

    /**
     * A function of this type is called to create command parsers while
     * parsing the pipeline. The function is called each time the pipeline
     * parser starts parsing a command contained in the pipeline.
     *
     * The function must return a new command parser that should be used to
     * parse the command starting from the current position of the environment.
     * If there is no valid command at the current position, either the command
     * parser pointer returned by the function or the command pointer returned
     * by the parser should be null.
     *
     * The function may peek a token at the current position to determine the
     * type of the command parser it creates. The function may remove line
     * continuations from the token, but may not modify the environment
     * otherwise.
     */
    using CommandParserCreator =
            std::function<std::unique_ptr<CommandParser>(Environment &)>;

private:

    enum class State {
        BEGINNING,
        SKIPPING_TO_COMMAND,
        COMMAND,
        PIPE,
    } mState;

    /** Not null (until {@link #parse()} returns successfully). */
    std::unique_ptr<syntax::Pipeline> mPipeline;

    /** Never null. */
    CommandParserCreator mCreateCommandParser;

    /** May be null. */
    std::unique_ptr<CommandParser> mCommandParser;

    common::Maybe<CommentSkipper> mSkipper;

public:

    /**
     * Creates a pipeline parser.
     * @param e An environment.
     * @param cpc (Must not be null) A command parser creator function.
     */
    PipelineParser(Environment &e, CommandParserCreator &&cpc);

    PipelineParser(const PipelineParser &) = delete;
    PipelineParser(PipelineParser &&) = default;
    PipelineParser &operator=(const PipelineParser &) = delete;
    PipelineParser &operator=(PipelineParser &&) = delete;
    ~PipelineParser() override = default;

private:

    bool parseCommand();

public:

    /**
     * Returns the parse result. The return value is a (nullable) pipeline
     * pointer. The null pointer means a parse error.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to finish parsing the command, this function
     * throws NeedMoreSource. In this case, the caller should set the EOF flag
     * or append to the source and then call this function again.
     *
     * @throws NeedMoreSource (see above)
     */
    std::unique_ptr<syntax::Pipeline> parse() override;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_PipelineParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
