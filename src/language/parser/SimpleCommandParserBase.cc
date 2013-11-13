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
#include "SimpleCommandParserBase.hh"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Predicate.hh"
#include "language/source/SourceBuffer.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

using sesh::language::syntax::Assignment;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;

namespace sesh {
namespace language {
namespace parser {

SimpleCommandParserBase::SimpleCommandParserBase(Environment &e) noexcept :
        Parser(),
        ParserBase(e),
        mState(State::of(normalCommentSkipper(e))),
        mCommand(new SimpleCommand) { }

void SimpleCommandParserBase::switchToCommentSkipper() {
    mState.emplace<CommentSkipper>(normalCommentSkipper(environment()));
}

void SimpleCommandParserBase::switchToAssignmentOrWordParser() {
    assert(mCommand != nullptr);
    if (mCommand->words().empty())
        mState.emplace<AssignmentParserPointer>(createAssignmentParser());
    else
        mState.emplace<WordParserPointer>(createWordParser(isTokenDelimiter));
}

class SimpleCommandParserBase::Processor {

private:

    SimpleCommandParserBase &mParser;

public:

    Processor(SimpleCommandParserBase &p) : mParser(p) { }

    using Result = bool;

    bool operator()(AssignmentPointer &&);
    bool operator()(WordPointer &&);

    bool operator()(CommentSkipper &);
    bool operator()(AssignmentParserPointer &);
    bool operator()(WordParserPointer &);

};

/** @return true if we should continue parsing. */
bool SimpleCommandParserBase::Processor::operator()(
        AssignmentPointer &&assignment) {
    assert(mParser.mCommand != nullptr);
    assert(assignment != nullptr);
    mParser.mCommand->assignments().emplace_back(std::move(assignment));

    mParser.switchToCommentSkipper();
    return true;
}

/** @return true if we should continue parsing. */
bool SimpleCommandParserBase::Processor::operator()(WordPointer &&word) {
    assert(word != nullptr);
    if (word->components().empty())
        return false;

    assert(mParser.mCommand != nullptr);
    mParser.mCommand->words().emplace_back(std::move(word));

    mParser.switchToCommentSkipper();
    return true;
}

/** @return true if we should continue parsing. */
bool SimpleCommandParserBase::Processor::operator()(CommentSkipper &skipper) {
    skipper.skip();
    mParser.switchToAssignmentOrWordParser();
    return true;
}

/** @return true if we should continue parsing. */
bool SimpleCommandParserBase::Processor::operator()(
        AssignmentParserPointer &parser) {
    assert(parser != nullptr);
    return parser->parse().apply(*this);
}

/** @return true if we should continue parsing. */
bool SimpleCommandParserBase::Processor::operator()(
        WordParserPointer &parser) {
    assert(parser != nullptr);
    return (*this)(parser->parse());
}

std::unique_ptr<syntax::SimpleCommand>
SimpleCommandParserBase::parseSimpleCommand() {
    if (mCommand == nullptr)
        throw std::logic_error("invalid parser state");

    while (mState.apply(Processor(*this))) { }

    return std::move(mCommand);
}

std::unique_ptr<syntax::Command> SimpleCommandParserBase::parse() {
    return parseSimpleCommand();
}

// TODO alias substitution

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
