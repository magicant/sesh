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
#include "TokenParser.hh"

#include <type_traits>
#include <utility>
#include "common/EnumSet.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/CharPredicates.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Keyword.hh"
#include "language/syntax/Word.hh"

using sesh::common::EnumSet;
using sesh::common::Maybe;
using sesh::common::String;
using sesh::language::syntax::Word;

namespace sesh {
namespace language {
namespace parser {

TokenParser::TokenParser(
        Environment &e, EnumSet<TokenType> acceptableTokenTypes) noexcept :
        Parser(e),
        mBlankAndCommentParser(e),
        mAcceptableTokenTypes(acceptableTokenTypes),
        mAssignmentParser(),
        mWordParser(),
        mResultToken(Token::create<WordPointer>()) { }

bool TokenParser::maybeParseAssignment() {
    if (!mAcceptableTokenTypes[TokenType::ASSIGNMENT])
        return false;

    if (mAssignmentParser == nullptr)
        mAssignmentParser = createAssignmentParser();

    AssignmentPointer *a = mAssignmentParser->parse();
    if (a == nullptr)
        return false;
    mResultToken.reset(std::move(*a));
    return true;
}

auto TokenParser::parseWord() -> WordPointer & {
    if (mWordParser == nullptr)
        mWordParser = createWordParser(isRawStringChar);

    WordPointer &word = *mWordParser->parse();
    if (word != nullptr && word->components().empty())
        word = nullptr;
    return word;
}

bool TokenParser::maybeParseKeyword() {
    if (!mAcceptableTokenTypes[TokenType::KEYWORD])
        return false;

    WordPointer &word = parseWord();
    if (word == nullptr || !word->isRawString())
        return false;

    Maybe<Keyword> k = Keyword::parse(word->maybeConstantValue().value());
    if (!k.hasValue())
        return false;
    mResultToken.reset(std::move(k.value()));
    return true;
}

bool TokenParser::maybeParseWord() {
    if (!mAcceptableTokenTypes[TokenType::WORD])
        return false;

    WordPointer &word = parseWord();
    if (word == nullptr)
        return false;
    mResultToken.reset(std::move(word));
    return true;
}

void TokenParser::parseImpl() {
    mBlankAndCommentParser.parse();

    if (maybeParseAssignment() || maybeParseKeyword() || maybeParseWord())
        result() = &mResultToken;
}

void TokenParser::resetImpl() noexcept {
    mBlankAndCommentParser.reset();
    if (mAssignmentParser != nullptr)
        mAssignmentParser->reset();
    if (mWordParser != nullptr)
        mWordParser->reset();
    mResultToken.emplace<WordPointer>();
    Parser::resetImpl();
}

void TokenParser::reset(EnumSet<TokenType> acceptableTokenTypes) noexcept {
    mAcceptableTokenTypes = acceptableTokenTypes;
    reset();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
