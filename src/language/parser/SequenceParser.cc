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
#include "SequenceParser.hh"

#include "common/EnumSet.hh"
#include "common/Maybe.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/Token.hh"

using sesh::common::EnumSet;
using sesh::common::Maybe;
using sesh::language::parser::Keyword;
using sesh::language::parser::Token;
using sesh::language::parser::TokenType;

namespace sesh {
namespace language {
namespace parser {

namespace {

bool isClosingKeyword(Keyword k) noexcept {
    return k == Keyword::keywordDo() ||
            k == Keyword::keywordDone() ||
            k == Keyword::keywordElif() ||
            k == Keyword::keywordElse() ||
            k == Keyword::keywordEsac() ||
            k == Keyword::keywordFi() ||
            k == Keyword::keywordThen() ||
            k == Keyword::keywordRightBrace();
}

} // namespace

SequenceParser::SequenceParser(Environment &e) noexcept :
        NormalParser<SequenceParserResult>(e),
        mInnerParser(InnerParser::create<TokenParserPointer>(nullptr)) { }

class SequenceParser::InnerParserProcessor {

private:

    SequenceParser &mParser;

    bool detectClosingKeyword(const Maybe<Token> &t) noexcept {
        if (!t.hasValue())
            return false;
        if (t->index() != t->index<Keyword>())
            return false;
        if (!isClosingKeyword(t->value<Keyword>()))
            return false;
        mParser.result()->second.emplace(t->value<Keyword>());
        return true;
    }

public:

    using Result = bool;

    InnerParserProcessor(SequenceParser &p) noexcept : mParser(p) { }

    bool operator()(TokenParserPointer &p) {
        if (p == nullptr)
            p = mParser.createTokenParser();
        if (detectClosingKeyword(p->parse()))
            return false;
        mParser.mInnerParser.reset(
                mParser.createAndOrListParser(std::move(p)));
        return true;
    }

    bool operator()(AndOrListParserPointer &p) {
        auto &r = p->parse();
        if (!r.hasValue())
            return false;
        mParser.result().value().first.andOrLists().push_back(
                std::move(r.value().first));
        if (!r.value().second)
            return false;
        mParser.mInnerParser.emplace<TokenParserPointer>();
        return true;
    }

}; // class SequenceParser::InnerParserProcessor

void SequenceParser::parseImpl() {
    if (!result().hasValue())
        result().emplace();
    while (mInnerParser.apply(InnerParserProcessor(*this))) { }
}

void SequenceParser::resetImpl() noexcept {
    mInnerParser.emplace<TokenParserPointer>(nullptr);
    NormalParser<SequenceParserResult>::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
