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

Maybe<ConditionalPipeline::Condition> toCondition(const Maybe<Operator> &o)
        noexcept {
    if (o.hasValue()) {
        if (o.value() == Operator::operatorAndAnd())
            return createMaybeOf(ConditionalPipeline::Condition::AND_THEN);
        if (o.value() == Operator::operatorPipePipe())
            return createMaybeOf(ConditionalPipeline::Condition::OR_ELSE);
    }
    return Maybe<ConditionalPipeline::Condition>();
}

} // namespace

AndOrListParser::ConditionalPipelineParser::ConditionalPipelineParser(
        Environment &e, Parser<PipelinePointer> &pp) :
        NormalParser<syntax::ConditionalPipeline>(e),
        mConditionParser(createOperatorParser(e)),
        mLinebreakParser(e),
        mPipelineParser(pp) { }

void AndOrListParser::ConditionalPipelineParser::parseImpl() {
    Maybe<ConditionalPipeline::Condition> condition =
            toCondition(mConditionParser.parse());
    if (!condition.hasValue())
        return;

    mLinebreakParser.parse();

    Maybe<PipelinePointer> &p = mPipelineParser.get().parse();
    if (!p.hasValue())
        return;

    result().emplace(condition.value(), std::move(p.value()));
}

void AndOrListParser::ConditionalPipelineParser::resetImpl() noexcept {
    mConditionParser.reset();
    mLinebreakParser.reset();
    mPipelineParser.get().reset();
    NormalParser<ConditionalPipeline>::resetImpl();
}

AndOrListParser::AndOrListParser(PipelineParserPointer &&pp) :
        NormalParser<Result>(pp->environment()),
        mPipelineParser(std::move(pp)),
        mConditionalPipelineListParser(
                environment(), environment(), *mPipelineParser),
        mSeparatorParser(createOperatorParser(environment())) { }

void AndOrListParser::parseImpl() {
    if (!result().hasValue()) {
        Maybe<PipelinePointer> &p = mPipelineParser->parse();
        if (!p.hasValue())
            return;
        result().emplace(
                std::unique_ptr<AndOrList>(
                        new AndOrList(std::move(*p.value()))),
                false);
        mPipelineParser->reset();
    }

    AndOrList &aol = *result().value().first;
    std::vector<ConditionalPipeline> &rest =
            mConditionalPipelineListParser.parse().value();

    if (Maybe<Operator> &o = mSeparatorParser.parse()) {
        if (*o == Operator::operatorSemicolon()) {
            aol.synchronicity() = AndOrList::Synchronicity::SEQUENTIAL;
            result().value().second = true;
        } else if (*o == Operator::operatorAnd()) {
            aol.synchronicity() = AndOrList::Synchronicity::ASYNCHRONOUS;
            result().value().second = true;
        } else
            environment().setPosition(mSeparatorParser.begin());
    }

    aol.rest() = std::move(rest);
}

void AndOrListParser::resetImpl() noexcept {
    mPipelineParser->reset();
    mConditionalPipelineListParser.reset();
    mSeparatorParser.reset();
    NormalParser<Result>::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
