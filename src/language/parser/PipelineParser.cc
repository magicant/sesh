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
#include "PipelineParser.hh"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/CommandParser.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/token.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::normalCommentSkipper;
using sesh::language::syntax::Pipeline;

namespace sesh {
namespace language {
namespace parser {

namespace {

const String BANG = L("!");
const String PIPE = L("|");

} // namespace

PipelineParser::PipelineParser(Environment &e, CommandParserCreator &&cpc) :
        Parser(e),
        mState(State::BEGINNING),
        mPipeline(new syntax::Pipeline),
        mCreateCommandParser(std::move(cpc)),
        mCommandParser(nullptr),
        mSkipper() {
    assert(mCreateCommandParser != nullptr);
}

bool PipelineParser::parseCommand() {
    if (mCommandParser == nullptr) {
        mCommandParser = mCreateCommandParser(environment());
        if (mCommandParser == nullptr)
            return false;
    }
    auto command = mCommandParser->parse();
    mCommandParser = nullptr;
    if (command == nullptr)
        return false;
    mPipeline->commands().emplace_back(std::move(command));
    return true;
}

std::unique_ptr<syntax::Pipeline> PipelineParser::parse() {
    assert(mPipeline != nullptr);

    switch (mState) {
    case State::BEGINNING:
        if (peekKeyword(environment()) == BANG) {
            environment().current() += BANG.length();
            mPipeline->exitStatusType() = Pipeline::ExitStatusType::NEGATED;

            mSkipper.emplace(normalCommentSkipper(environment()));
changeStateSkippingToCommand:
            mState = State::SKIPPING_TO_COMMAND;
            // fall-through
    case State::SKIPPING_TO_COMMAND:
            mSkipper->skip();
            mSkipper.clear();

            if (peekKeyword(environment()) == BANG) {
                // TODO report error
                return nullptr;
            }
        }
        mState = State::COMMAND;
        // fall-through
    case State::COMMAND:
        if (!parseCommand())
            return nullptr;
        mState = State::PIPE;
        // fall-through
    case State::PIPE:
        if (peekSymbol(environment()) == PIPE) {
            environment().current() += PIPE.length();
            mSkipper.emplace(whitespaceSkipper(environment()));
            goto changeStateSkippingToCommand;
        }
    } // switch

    if (mPipeline->commands().empty()) {
        // TODO report error
        return nullptr;
    }
    return std::move(mPipeline);
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
