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

AssignedWordParser::AssignedWordParser(WordParserPointer &&wordParser) :
        Parser(wordParser->environment()),
        mWordParser(std::move(wordParser)),
        mColonParser(mWordParser->environment(), isColon),
        mResultWord() { }

bool AssignedWordParser::parseComponent() {
    WordPointer &word = *mWordParser->parse();
    auto *colon = mColonParser.parse();

    if (mResultWord != nullptr)
        mResultWord->append(std::move(*word));
    else
        mResultWord = std::move(word);
    mWordParser->reset();

    if (colon == nullptr)
        return false;
    mResultWord->addComponent(Word::ComponentPointer(
            new RawString(String(1, *colon))));
    mColonParser.reset();
    return true;
}

void AssignedWordParser::parseImpl() {
    while (parseComponent()) { }
    result() = &mResultWord;
}

void AssignedWordParser::resetImpl() noexcept {
    mWordParser->reset();
    mColonParser.reset();
    mResultWord.reset();
    Parser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
