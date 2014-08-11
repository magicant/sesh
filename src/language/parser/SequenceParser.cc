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
#include "language/parser/Environment.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/Token.hh"
#include "language/syntax/Sequence.hh"

using sesh::common::EnumSet;
using sesh::common::Maybe;
using sesh::language::parser::Keyword;
using sesh::language::parser::Token;
using sesh::language::parser::TokenType;
using sesh::language::syntax::Sequence;

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
        Parser<SequenceParserResult>(e),
        mInnerParser(InnerParser::create<TokenParserPointer>(nullptr)),
        mResult() { }

class SequenceParser::InnerParserProcessor {

private:

    SequenceParser &mParser;

    bool detectClosingKeyword(const Token *t, Environment::Size position)
            noexcept {
        if (t == nullptr)
            return false;
        if (t->tag() != t->tag<Keyword>())
            return false;
        if (!isClosingKeyword(t->value<Keyword>()))
            return false;
        mParser.mResult.second.emplace(t->value<Keyword>(), position);
        return true;
    }

public:

    InnerParserProcessor(SequenceParser &p) noexcept : mParser(p) { }

    bool operator()(TokenParserPointer &p) {
        if (p == nullptr)
            p = mParser.createTokenParser();
        auto *token = p->parse();
        if (detectClosingKeyword(token, p->begin()))
            return false;
        mParser.mInnerParser.reset(
                mParser.createAndOrListParser(std::move(p)));
        return true;
    }

    bool operator()(AndOrListParserPointer &p) {
        auto *r = p->parse();
        if (r == nullptr)
            return false;
        mParser.mResult.first.andOrLists().push_back(std::move(r->first));
        if (!r->second)
            return false;
        mParser.mInnerParser.emplace<TokenParserPointer>();
        return true;
    }

}; // class SequenceParser::InnerParserProcessor

void SequenceParser::parseImpl() {
    while (mInnerParser.apply(InnerParserProcessor(*this))) { }
    result() = &mResult;
}

void SequenceParser::resetImpl() noexcept {
    mInnerParser.emplace<TokenParserPointer>(nullptr);
    mResult.first = Sequence();
    mResult.second.clear();
    Parser<SequenceParserResult>::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
