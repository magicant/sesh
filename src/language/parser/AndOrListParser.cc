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

#include "buildconfig.h"
#include "AndOrListParser.hh"

#include <memory>
#include <utility>
#include <vector>
#include "common/Maybe.hh"
#include "language/parser/Operator.hh"
#include "language/parser/OperatorParser.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/ConditionalPipeline.hh"

using sesh::common::Maybe;
using sesh::common::createMaybeOf;
using sesh::language::parser::Operator;
using sesh::language::parser::createOperatorParser;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::ConditionalPipeline;

namespace sesh {
namespace language {
namespace parser {

namespace {

Maybe<ConditionalPipeline::Condition> toCondition(const Operator *o) noexcept {
    if (o != nullptr) {
        if (*o == Operator::operatorAndAnd())
            return createMaybeOf(ConditionalPipeline::Condition::AND_THEN);
        if (*o == Operator::operatorPipePipe())
            return createMaybeOf(ConditionalPipeline::Condition::OR_ELSE);
    }
    return Maybe<ConditionalPipeline::Condition>();
}

} // namespace

AndOrListParser::ConditionalPipelineParser::ConditionalPipelineParser(
        Environment &e, Parser<PipelinePointer> &pp) :
        Parser(e),
        mConditionParser(createOperatorParser(e)),
        mLinebreakParser(e),
        mPipelineParser(pp),
        mResultPipeline() { }

void AndOrListParser::ConditionalPipelineParser::parseImpl() {
    Maybe<ConditionalPipeline::Condition> condition =
            toCondition(mConditionParser.parse());
    if (!condition.hasValue())
        return;

    mLinebreakParser.parse();

    PipelinePointer *p = mPipelineParser.get().parse();
    if (p == nullptr)
        return;

    mResultPipeline.emplace(condition.value(), std::move(*p));
    result() = &mResultPipeline.value();
}

void AndOrListParser::ConditionalPipelineParser::resetImpl() noexcept {
    mConditionParser.reset();
    mLinebreakParser.reset();
    mPipelineParser.get().reset();
    mResultPipeline.clear();
    Parser::resetImpl();
}

AndOrListParser::AndOrListParser(PipelineParserPointer &&pp) :
        Parser(pp->environment()),
        mPipelineParser(std::move(pp)),
        mConditionalPipelineListParser(
                environment(), environment(), *mPipelineParser),
        mSeparatorParser(createOperatorParser(environment())),
        mResult() { }

void AndOrListParser::parseImpl() {
    if (mResult.first == nullptr) {
        PipelinePointer *p = mPipelineParser->parse();
        if (p == nullptr)
            return;
        mResult.first.reset(new AndOrList(std::move(**p)));
        mPipelineParser->reset();
        mResult.second = false;
        result() = &mResult;
    }

    AndOrList &aol = *mResult.first;
    std::vector<ConditionalPipeline> &rest =
            *mConditionalPipelineListParser.parse();

    if (const Operator *o = mSeparatorParser.parse()) {
        if (*o == Operator::operatorSemicolon()) {
            aol.synchronicity() = AndOrList::Synchronicity::SEQUENTIAL;
            mResult.second = true;
        } else if (*o == Operator::operatorAnd()) {
            aol.synchronicity() = AndOrList::Synchronicity::ASYNCHRONOUS;
            mResult.second = true;
        } else
            environment().setPosition(mSeparatorParser.begin());
    }

    aol.rest() = std::move(rest);
}

void AndOrListParser::resetImpl() noexcept {
    mPipelineParser->reset();
    mConditionalPipelineListParser.reset();
    mSeparatorParser.reset();
    mResult.first.reset();
    Parser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
