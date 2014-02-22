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

#ifndef INCLUDED_language_parser_AndOrListParser_hh
#define INCLUDED_language_parser_AndOrListParser_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "language/parser/Environment.hh"
#include "language/parser/OperatorParser.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Repeat.hh"
#include "language/syntax/ConditionalPipeline.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Pipeline.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses an and-or list.
 *
 * This is an abstract class that implements some part of the parser. A
 * concrete subclass must provide factory methods that create parsers used by
 * this parser.
 */
class AndOrListParser :
        public NormalParser<std::unique_ptr<syntax::AndOrList>> {

public:

    using AndOrListPointer = std::unique_ptr<syntax::AndOrList>;
    using PipelinePointer = std::unique_ptr<syntax::Pipeline>;

    using PipelineParserPointer = std::unique_ptr<Parser<PipelinePointer>>;

private:

    class ConditionalPipelineParser :
            public NormalParser<syntax::ConditionalPipeline> {

    private:

        OperatorParser mConditionParser;
        std::reference_wrapper<Parser<PipelinePointer>> mPipelineParser;

    public:

        ConditionalPipelineParser(Environment &e, Parser<PipelinePointer> &pp);

    private:

        void parseImpl() final override;

        void resetImpl() noexcept final override;

    }; // class ConditionalPipelineParser

    PipelineParserPointer mPipelineParser;
    Repeat<ConditionalPipelineParser> mConditionalPipelineListParser;
    OperatorParser mSeparatorParser;

public:

    /**
     * Constructs a new and-or list parser.
     *
     * The argument pipeline parser is used to parse every pipeline in the
     * and-or list. The pipeline parser does not have to be in the "unstarted"
     * state: it may already be "parsing" the first pipeline or even
     * "finished". This constructor does not reset the pipeline parser but the
     * reset method does.
     */
    AndOrListParser(PipelineParserPointer &&);

    AndOrListParser(AndOrListParser &&) = default;
    AndOrListParser&operator=(AndOrListParser &&) = default;

private:

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class AndOrListParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AndOrListParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
