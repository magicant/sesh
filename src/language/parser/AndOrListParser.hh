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

#ifndef INCLUDED_language_parser_AndOrListParser_hh
#define INCLUDED_language_parser_AndOrListParser_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "common/Maybe.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/Pipeline.hh"

namespace sesh {
namespace language {
namespace parser {

class AndOrListParser final :
        public Parser<std::unique_ptr<syntax::AndOrList>>,
        protected ParserBase {

public:

    using PipelineParserPointer =
            std::unique_ptr<Parser<std::unique_ptr<syntax::Pipeline>>>;

    /**
     * A function of this type is called to create pipeline parsers while
     * parsing the and-or list. The function is called each time the and-or
     * list parser starts parsing a pipeline contained in the and-or list.
     *
     * The function must return a new pipeline parser that should be used to
     * parse the pipeline starting from the current position of the
     * environment. If there is no valid pipeline at the current position, the
     * returned parser should return null.
     *
     * The function may peek a token at the current position to determine the
     * type of the command parser it creates. The function may remove line
     * continuations from the token, but may not modify the environment
     * otherwise.
     */
    using PipelineParserCreator =
            std::function<PipelineParserPointer(Environment &)>;

private:

    /** Null until the first pipeline has been parsed. */
    std::unique_ptr<syntax::AndOrList> mAndOrList;

    /** Never null. */
    PipelineParserCreator mCreatePipelineParser;

    /** Not null while parsing a pipeline. */
    PipelineParserPointer mPipelineParser;

    /**
     * Not null after a "&&" or "||" token has parsed until the following
     * pipeline has been parsed.
     */
    common::Maybe<syntax::ConditionalPipeline::Condition> mCondition;

    /** Skips whitespaces after "&&" and "||". */
    CommentSkipper mSkipper;

public:

    /**
     * Creates an and-or list parser.
     * @param e An environment.
     * @param ppc (Must not be null) A pipeline parser creator function.
     */
    AndOrListParser(Environment &e, PipelineParserCreator &&ppc);

    AndOrListParser(const AndOrListParser &) = delete;
    AndOrListParser(AndOrListParser &&) = default;
    AndOrListParser& operator=(const AndOrListParser &) = delete;
    AndOrListParser& operator=(AndOrListParser &&) = delete;
    ~AndOrListParser() override = default;

private:

    bool parsePipeline();
    void parseSymbol();

public:

    /**
     * Parses the and-or list. Returns null on a parse error.
     *
     * The environment's current position is advanced past the parsed and-or
     * list.
     *
     * If this function returns a result without throwing, the internal state
     * of this parser is no longer valid and this function must never be called
     * again.
     *
     * If more source is needed to finish parsing the command, this function
     * throws NeedMoreSource. In this case, the caller should set the EOF flag
     * or append to the source and then call this function again.
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    std::unique_ptr<syntax::AndOrList> parse() override;

}; // class AndOrListParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AndOrListParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
