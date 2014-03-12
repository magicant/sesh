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
#include "LineSequenceParser.hh"

#include <cassert>
#include <utility>
#include "common/Char.hh"
#include "common/ErrorLevel.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "i18n/M.h"
#include "language/parser/EnvironmentHelper.hh"
#include "language/parser/IncompleteParse.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/Operator.hh"
#include "language/parser/OperatorParser.hh"
#include "language/parser/PositionedKeyword.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::ErrorLevel;
using sesh::common::Maybe;
using sesh::common::Message;
using sesh::common::String;

namespace sesh {
namespace language {
namespace parser {

namespace {

Message<> errorMessageForOperator(Operator o) {
    if (o == Operator::operatorRightParenthesis())
        return Message<String, String>(
                L(M("encountered `%1%' without a matching `%2%'"))) %
                o %
                Operator::LEFT_PARENTHESIS;
    // XXX check for other operators?

    return Message<String>(L(M("unexpected symbol `%1%'"))) % o;
}

Message<> errorMessageForClosingKeyword(Keyword k) {
    if (k == Keyword::keywordDo())
        return Message<>(L(
        M("encountered `do' without a matching `for', `while', or `until'")));
    if (k == Keyword::keywordDone())
        return Message<>(L(M("encountered `do' without a matching `do'")));
    if (k == Keyword::keywordElif() || k == Keyword::keywordElse() ||
            k == Keyword::keywordFi())
        return Message<String, String>(
                L(M("encountered `%1%' without a matching `%2%'"))) %
                k %
                Keyword::IF;
    if (k == Keyword::keywordThen())
        return Message<>(
                L(M("encountered `then' without a matching `if' or `elif'")));
    if (k == Keyword::keywordRightBrace())
        return Message<String, String>(
                L(M("encountered `%1%' without a matching `%2%'"))) %
                k %
                Keyword::LEFT_BRACE;

    throw k; // unexpected
}

Message<> errorMessageForInvalidCharacter(Char c) {
    switch (c) {
    case L('\0'):
        return Message<>(L(M("unexpected null character")));
    case L('\r'):
        return Message<>(L(M("unexpected carriage return")));
    default:
        return Message<Char>(L(M("unrecognized character `%1%'"))) % c;
    }
}

} // namespace

LineSequenceParser::LineSequenceParser(
        SequenceParserPointer &&sp, NewlineParserPointer &&np) noexcept :
        Parser(sp->environment()),
        mSequenceParser(std::move(sp)),
        mNewlineParser(std::move(np)),
        mOperatorParser(createOperatorParser(environment())) { }

void LineSequenceParser::parseNonNewlineTrailer() {
    if (const Operator *o = mOperatorParser.parse())
        return environment().addDiagnosticMessage(
                mOperatorParser.begin(),
                errorMessageForOperator(*o),
                ErrorLevel::ERROR);

    auto ci = charIntAt(environment(), environment().position());
    if (!CharTraits::eq_int_type(ci, CharTraits::eof()))
        return environment().addDiagnosticMessage(
                environment().position(),
                errorMessageForInvalidCharacter(CharTraits::to_char_type(ci)),
                ErrorLevel::ERROR);

    // TODO error if any here-document pending
}

void LineSequenceParser::parseImpl() {
    SequenceParserResult *spr = mSequenceParser->parse();
    assert(spr != nullptr);

    if (mNewlineParser->parse() == nullptr)
        parseNonNewlineTrailer();

    result() = &spr->first;

    if (Maybe<PositionedKeyword> &pk = spr->second)
        environment().addDiagnosticMessage(
                pk->position(),
                errorMessageForClosingKeyword(pk->keyword()),
                ErrorLevel::ERROR);
}

void LineSequenceParser::resetImpl() noexcept {
    mSequenceParser->reset();
    mNewlineParser->reset();
    mOperatorParser.reset();
    Parser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
