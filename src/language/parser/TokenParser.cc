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

namespace {

template<typename V>
void emplace(Maybe<Token> &t, V &&v) {
    using sesh::common::variant_impl::TypeTag;
    t.emplace(TypeTag<typename std::decay<V>::type>(), std::forward<V>(v));
}

} // namespace

TokenParser::TokenParser(
        Environment &e, EnumSet<TokenType> acceptableTokenTypes) noexcept :
        NormalParser(e),
        mBlankAndCommentParser(e),
        mAcceptableTokenTypes(acceptableTokenTypes),
        mAssignmentParser(),
        mWordParser() { }

bool TokenParser::maybeParseAssignment() {
    if (!mAcceptableTokenTypes[TokenType::ASSIGNMENT])
        return false;

    if (mAssignmentParser == nullptr)
        mAssignmentParser = createAssignmentParser();

    Maybe<AssignmentPointer> &a = mAssignmentParser->parse();
    if (!a.hasValue())
        return false;
    emplace(result(), std::move(a.value()));
    return true;
}

auto TokenParser::parseWord() -> WordPointer & {
    if (mWordParser == nullptr)
        mWordParser = createWordParser(isRawStringChar);

    WordPointer &word = mWordParser->parse().value();
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
    emplace(result(), std::move(k.value()));
    return true;
}

bool TokenParser::maybeParseWord() {
    if (!mAcceptableTokenTypes[TokenType::WORD])
        return false;

    WordPointer &word = parseWord();
    if (word == nullptr)
        return false;
    emplace(result(), std::move(word));
    return true;
}

void TokenParser::parseImpl() {
    mBlankAndCommentParser.parse();

    if (maybeParseAssignment())
        return;
    if (maybeParseKeyword())
        return;
    if (maybeParseWord())
        return;
}

void TokenParser::resetImpl() noexcept {
    mBlankAndCommentParser.reset();
    if (mAssignmentParser != nullptr)
        mAssignmentParser->reset();
    if (mWordParser != nullptr)
        mWordParser->reset();
    NormalParser::resetImpl();
}

void TokenParser::reset(EnumSet<TokenType> acceptableTokenTypes) noexcept {
    mAcceptableTokenTypes = acceptableTokenTypes;
    reset();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
