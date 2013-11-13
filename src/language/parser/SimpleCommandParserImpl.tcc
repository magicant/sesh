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

#ifndef INCLUDED_language_parser_SimpleCommandParserImpl_tcc
#define INCLUDED_language_parser_SimpleCommandParserImpl_tcc

#include "buildconfig.h"
#include "SimpleCommandParserImpl.hh"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Predicate.hh"
#include "language/source/SourceBuffer.hh"
#include "language/syntax/SimpleCommand.hh"

namespace sesh {
namespace language {
namespace parser {

template<typename Types>
SimpleCommandParserImpl<Types>::SimpleCommandParserImpl(Environment &e)
        noexcept :
        ParserBase(e),
        Parser(),
        mState(State::of(normalCommentSkipper(e))),
        mCommand(new syntax::SimpleCommand) { }

template<typename Types>
void SimpleCommandParserImpl<Types>::switchToCommentSkipper() {
    mState.template emplace<CommentSkipper>(
            normalCommentSkipper(environment()));
}

template<typename Types>
void SimpleCommandParserImpl<Types>::switchToAssignmentOrWordParser() {
    assert(mCommand != nullptr);
    if (mCommand->words().empty())
        mState.template emplace<AssignmentParser>(environment());
    else
        mState.template emplace<WordParser>(environment(), isTokenDelimiter);
}

/** @return true if we should continue parsing. */
template<typename Types>
bool SimpleCommandParserImpl<Types>::Processor::operator()(
        typename AssignmentParser::AssignmentPointer &&assignment) {
    assert(mParser.mCommand != nullptr);
    assert(assignment != nullptr);
    mParser.mCommand->assignments().emplace_back(std::move(assignment));

    mParser.switchToCommentSkipper();
    return true;
}

/** @return true if we should continue parsing. */
template<typename Types>
bool SimpleCommandParserImpl<Types>::Processor::operator()(
        typename AssignmentParser::WordPointer &&word) {
    assert(word != nullptr);
    if (word->components().empty())
        return false;

    assert(mParser.mCommand != nullptr);
    mParser.mCommand->words().emplace_back(std::move(word));

    mParser.switchToCommentSkipper();
    return true;
}

/** @return true if we should continue parsing. */
template<typename Types>
bool SimpleCommandParserImpl<Types>::Processor::operator()(
        CommentSkipper &skipper) {
    skipper.skip();
    mParser.switchToAssignmentOrWordParser();
    return true;
}

/** @return true if we should continue parsing. */
template<typename Types>
bool SimpleCommandParserImpl<Types>::Processor::operator()(
        AssignmentParser &parser) {
    return parser.parse().apply(*this);
}

/** @return true if we should continue parsing. */
template<typename Types>
bool SimpleCommandParserImpl<Types>::Processor::operator()(
        WordParser &parser) {
    return (*this)(parser.parse());
}

template<typename Types>
std::unique_ptr<syntax::SimpleCommand>
SimpleCommandParserImpl<Types>::parseSimpleCommand() {
    if (mCommand == nullptr)
        throw std::logic_error("invalid parser state");

    while (mState.apply(Processor(*this))) { }

    return std::move(mCommand);
}

template<typename Types>
std::unique_ptr<syntax::Command> SimpleCommandParserImpl<Types>::parse() {
    return parseSimpleCommand();
}

// TODO alias substitution

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SimpleCommandParserImpl_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
