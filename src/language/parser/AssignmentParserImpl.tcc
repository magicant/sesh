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

#ifndef INCLUDED_language_parser_AssignmentParserImpl_tcc
#define INCLUDED_language_parser_AssignmentParserImpl_tcc

#include "buildconfig.h"
#include "AssignmentParserImpl.hh"

#include <cassert>
#include <functional>
#include <locale>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "common/Variant.hh"
#include "language/parser/Predicate.hh"
#include "language/syntax/Assignment.hh"

namespace sesh {
namespace language {
namespace parser {

template<typename Types>
AssignmentParserImpl<Types>::AssignmentParserImpl(Environment &e) :
        Parser(e),
        mBegin(e.current()),
        mVariableName(),
        mState(State::template create<NameParser>(
                e,
                std::not2(Predicate<common::Char>(isVariableNameChar)))) { }

template<typename Types>
bool AssignmentParserImpl<Types>::isValidVariableName(
        const Environment &e, const common::String &s) {
    return !s.empty() && !std::isdigit(s[0], e.locale());
}

template<typename Types>
void AssignmentParserImpl<Types>::parseVariableName() {
    assert(mState.index() == mState.template index<NameParser>());

    common::String name = mState.template value<NameParser>().parse();

    if (isValidVariableName(environment(), name) &&
            currentCharInt() == common::CharTraits::to_int_type(L('='))) {
        mVariableName.emplace(std::move(name));
        ++environment().current();
    } else {
        // No variable name; The whole token is a word.
        environment().current() = mBegin;
    }
}

template<typename Types>
auto AssignmentParserImpl<Types>::parse() -> Result {
    if (mState.index() == mState.template index<NameParser>()) {
        parseVariableName();
        mState.template emplace<WordParser>(environment(), isTokenDelimiter);
    }

    WordPointer word = mState.template value<WordParser>().parse();
    assert(word != nullptr);
    if (mVariableName.hasValue())
        return Result::of(AssignmentPointer(new syntax::Assignment(
                std::move(mVariableName).value(), std::move(word))));
    else
        return Result::of(WordPointer(std::move(word)));
}

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParserImpl_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
