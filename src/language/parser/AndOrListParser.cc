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

#include "buildconfig.h"
#include "AndOrListParser.hh"

#include <cassert>
#include <memory>
#include <utility>
#include "language/parser/CommentSkipper.hh"
#include "language/parser/token.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/ConditionalPipeline.hh"

using sesh::language::parser::Environment;
using sesh::language::parser::Parser;
using sesh::language::parser::ParserBase;
using sesh::language::syntax::AndOrList;

using Synchronicity = sesh::language::syntax::AndOrList::Synchronicity;
using Condition = sesh::language::syntax::ConditionalPipeline::Condition;

namespace sesh {
namespace language {
namespace parser {

AndOrListParser::AndOrListParser(Environment &e, PipelineParserCreator &&ppc) :
        Parser(),
        ParserBase(e),
        mAndOrList(nullptr),
        mCreatePipelineParser(std::move(ppc)),
        mPipelineParser(mCreatePipelineParser(environment())),
        mCondition(),
        mSkipper(whitespaceSkipper(environment())) { }

bool AndOrListParser::parsePipeline() {
    assert(mPipelineParser != nullptr);

    auto pipeline = mPipelineParser->parse();
    mPipelineParser = nullptr;
    if (pipeline == nullptr)
        return false;

    if (mAndOrList == nullptr)
        mAndOrList.reset(new AndOrList(std::move(*pipeline)));
    else {
        mAndOrList->rest().emplace_back(
                mCondition.value(), std::move(pipeline));
        mCondition.clear();
    }
    return true;
}

void AndOrListParser::parseSymbol() {
    const auto &symbol = peekSymbol(environment());
    if (symbol == token::AND_AND)
        mCondition = Condition::AND_THEN;
    else if (symbol == token::PIPE_PIPE)
        mCondition = Condition::OR_ELSE;
    else if (symbol == token::SEMICOLON)
        mAndOrList->synchronicity() = Synchronicity::SEQUENTIAL;
    else if (symbol == token::AND)
        mAndOrList->synchronicity() = Synchronicity::ASYNCHRONOUS;
    else
        return;
    environment().current() += symbol.length();
}

std::unique_ptr<AndOrList> AndOrListParser::parse() {
    for (;;) {
        if (mPipelineParser != nullptr)
            if (!parsePipeline())
                return nullptr;

        if (!mCondition.hasValue()) {
            parseSymbol();
            if (!mCondition.hasValue())
                return std::move(mAndOrList);
        }
        mSkipper.skip();
        mPipelineParser = mCreatePipelineParser(environment());
    }
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
