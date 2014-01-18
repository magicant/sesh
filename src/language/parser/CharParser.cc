/* Copyright (C) 2013-2014 WATANABE Yuki
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
#include "CharParser.hh"

#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/LineContinuationTreatment.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::Maybe;

namespace sesh {
namespace language {
namespace parser {

CharParser::CharParser(
        Environment &e,
        Predicate<Char> &&isAcceptableChar,
        LineContinuationTreatment lct) :
        NormalParser(e),
        mIsAcceptableChar(std::move(isAcceptableChar)),
        mLineContinuationTreatment(lct) { }

namespace {

void removeLineContinuations(Environment &e) {
    while (e.removeLineContinuation(e.position())) { }
}

} // namespace

void CharParser::parseImpl() {
    switch (mLineContinuationTreatment) {
    case LineContinuationTreatment::LITERAL:
        break;
    case LineContinuationTreatment::REMOVE:
        removeLineContinuations(environment());
        break;
    }

    CharInt i = currentCharInt();
    if (CharTraits::eq_int_type(i, CharTraits::eof()))
        return;
    if (mIsAcceptableChar == nullptr)
        return;

    Char c = CharTraits::to_char_type(i);
    if (!mIsAcceptableChar(environment(), c))
        return;

    result() = c;
    environment().setPosition(environment().position() + 1);
}

void CharParser::reset(
        Predicate<Char> &&isAcceptableChar,
        LineContinuationTreatment lct) {
    mIsAcceptableChar = std::move(isAcceptableChar);
    mLineContinuationTreatment = lct;
    reset();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */