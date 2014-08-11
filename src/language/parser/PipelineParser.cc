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
#include "PipelineParser.hh"

#include <memory>
#include <utility>
#include "language/parser/LinebreakParser.hh"
#include "language/parser/Operator.hh"
#include "language/parser/Parser.hh"
#include "language/parser/SpecificOperatorParser.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"

using sesh::language::parser::LinebreakParser;
using sesh::language::parser::SpecificOperatorParser;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;

namespace sesh {
namespace language {
namespace parser {

namespace {

/** Parses a command preceded by a pipe operator. */
class PipedCommandParser : public Parser<std::unique_ptr<Command>> {

public:

    using CommandPointer = std::unique_ptr<Command>;
    using CommandParserPointer = std::unique_ptr<Parser<CommandPointer>>;

private:

    SpecificOperatorParser mPipeOperatorParser;
    LinebreakParser mLinebreakParser;
    CommandParserPointer mCommandParser;

public:

    explicit PipedCommandParser(CommandParserPointer cp) :
            Parser(cp->environment()),
            mPipeOperatorParser(cp->environment(), Operator::operatorPipe()),
            mLinebreakParser(cp->environment()),
            mCommandParser(std::move(cp)) { }

    void parseImpl() final override {
        if (mPipeOperatorParser.parse() != nullptr) {
            mLinebreakParser.parse();
            result() = mCommandParser->parse();
        }
    }

    void resetImpl() noexcept final override {
        mPipeOperatorParser.reset();
        mLinebreakParser.reset();
        mCommandParser->reset();
        Parser<CommandPointer>::resetImpl();
    }

}; // class PipedCommandParser

} // namespace

PipelineParser::PipelineParser(Environment &e, TokenParserPointer &&tp) :
        Parser<PipelinePointer>(e),
        mState(ParseState::create<ParsingFirstToken>()) {
    mState.value<ParsingFirstToken>().tokenParser = std::move(tp);
}

class PipelineParser::StateProcessor {

private:

    PipelineParser &mParser;

public:

    StateProcessor(PipelineParser &p) noexcept : mParser(p) { }

    bool operator()(ParsingFirstToken &state) {
        if (state.tokenParser == nullptr)
            state.tokenParser = mParser.createTokenParser();

        Token *t = state.tokenParser->parse();
        if (t == nullptr)
            return false;

        Pipeline::ExitStatusType est;
        if (t->index() == t->index<Keyword>() &&
                t->value<Keyword>() == Keyword::keywordExclamation()) {
            est = Pipeline::ExitStatusType::NEGATED;
            state.tokenParser->reset();
        } else {
            est = Pipeline::ExitStatusType::STRAIGHT;
        }

        ParsingFirstCommand newState;
        newState.pipeline = PipelinePointer(new Pipeline(est));
        newState.commandParser =
                mParser.createCommandParser(std::move(state.tokenParser));
        mParser.mState.assign(std::move(newState));
        return true;
    }

    bool operator()(ParsingFirstCommand &state) {
        CommandPointer *c = state.commandParser->parse();
        if (c == nullptr)
            return false;
        state.pipeline->commands().push_back(std::move(*c));
        state.commandParser->reset();

        ParsingRemainingCommands newState;
        newState.pipeline = std::move(state.pipeline);
        newState.pipedCommandParser = CommandParserPointer(
                new PipedCommandParser(std::move(state.commandParser)));
        mParser.mState.assign(std::move(newState));
        return true;
    }

    bool operator()(ParsingRemainingCommands &state) {
        if (CommandPointer *c = state.pipedCommandParser->parse()) {
            state.pipeline->commands().push_back(std::move(*c));
            state.pipedCommandParser->reset();
            return true;
        }
        mParser.result() = &state.pipeline;
        return false;
    }

}; // class PipelinePointer::StateProcessor

void PipelineParser::parseImpl() {
    while (mState.apply(StateProcessor(*this))) { }
}

void PipelineParser::resetImpl() noexcept {
    mState.emplace<ParsingFirstToken>();
    Parser<PipelinePointer>::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
