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
#include "SequenceParser.hh"

#include <functional>
#include <memory>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/LineMode.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Skipper.hh"
#include "language/parser/token.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Sequence.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::String;
using sesh::language::parser::CommentSkipper;
using sesh::language::parser::Predicate;
using sesh::language::parser::Skipper;
using sesh::language::parser::isNewline;
using sesh::language::parser::normalCommentSkipper;
using sesh::language::parser::whitespaceSkipper;
using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Sequence;

namespace sesh {
namespace language {
namespace parser {

namespace {

CommentSkipper skipper(Environment &e, LineMode lm) {
    switch (lm) {
    case LineMode::SINGLE_LINE:
        return normalCommentSkipper(e);
    case LineMode::MULTI_LINE:
        return whitespaceSkipper(e);
    }
}

bool isStopWord(Environment &e) {
    if (e.current() == e.end() && e.isEof())
        return true;

    const String &symbol = peekSymbol(e);
    if (symbol == token::RIGHT_PARENTHESIS ||
            symbol == token::SEMICOLON_SEMICOLON)
        return true;

    const String &keyword = peekKeyword(e);
    return keyword == token::RIGHT_BRACE ||
            keyword == token::DO ||
            keyword == token::DONE ||
            keyword == token::ELIF ||
            keyword == token::ELSE ||
            keyword == token::ESAC ||
            keyword == token::FI ||
            keyword == token::THEN;
}

} // namespace

SequenceParser::SequenceParser(
        Environment &e, AndOrListParserCreator &&pc, LineMode lm) :
        Parser(),
        ParserBase(e),
        mSequence(new Sequence),
        mCreateAndOrListParser(std::move(pc)),
        mAndOrListParser(nullptr),
        mWhitespaceSkipper(skipper(e, lm)) { }

bool SequenceParser::skipNewline() {
    if (!CharTraits::eq_int_type(
                currentCharInt(), CharTraits::to_int_type(L('\n'))))
        return false;

    ++environment().current();
    // TODO parse pending redirections
    return true;
}

std::unique_ptr<Sequence> SequenceParser::parse() {
    for (;;) {
        if (mAndOrListParser == nullptr) {
            mWhitespaceSkipper.skip();
            if (skipNewline() || isStopWord(environment()))
                return std::move(mSequence);

            mAndOrListParser = mCreateAndOrListParser(environment());
            if (mAndOrListParser == nullptr)
                return nullptr;
        }

        std::unique_ptr<AndOrList> l = mAndOrListParser->parse();
        mAndOrListParser = nullptr;
        if (l == nullptr)
            return nullptr;
        mSequence->andOrLists().push_back(std::move(l));
    }
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
