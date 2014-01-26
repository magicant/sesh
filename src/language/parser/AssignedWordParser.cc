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
#include "AssignedWordParser.hh"

#include <algorithm>
#include <iterator>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::language::syntax::RawString;
using sesh::language::syntax::Word;

namespace sesh {
namespace language {
namespace parser {

namespace {

constexpr bool isColon(const Environment &, Char c) noexcept {
    return c == L(':');
}

} // namespace

AssignedWordParser::AssignedWordParser(
        Environment &e, WordParserPointer wordParser) :
        NormalParser(e),
        mState(State::WORD),
        mWordParser(std::move(wordParser)),
        mColonParser(e, isColon) { }

bool AssignedWordParser::parseWord() {
    WordPointer word = std::move(mWordParser->parse().value());
    if (result().hasValue())
        result().value()->append(std::move(*word));
    else
        result().emplace(std::move(word));
    mWordParser->reset();
    mState = State::COLON;
    return true;
}

bool AssignedWordParser::parseColon() {
    auto &maybeColon = mColonParser.parse();
    if (!maybeColon.hasValue())
        return false;
    result().value()->addComponent(Word::ComponentPointer(
            new RawString(String(1, maybeColon.value()))));
    mColonParser.reset();
    mState = State::WORD;
    return true;
}

bool AssignedWordParser::parseComponent() {
    switch (mState) {
    case State::WORD:
        return parseWord();
    case State::COLON:
        return parseColon();
    }
}

void AssignedWordParser::parseImpl() {
    while (parseComponent()) { }
}

void AssignedWordParser::resetImpl() noexcept {
    mState = State::WORD;
    mWordParser->reset();
    mColonParser.reset();
    NormalParser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
