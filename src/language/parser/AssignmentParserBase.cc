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
#include "AssignmentParserBase.hh"

#include <cassert>
#include <functional>
#include <locale>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "common/Variant.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/Assignment.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::parser::StringParser;

namespace sesh {
namespace language {
namespace parser {

AssignmentParserBase::AssignmentParserBase(Environment &e) :
        Parser(),
        ParserBase(e),
        mBegin(e.current()),
        mVariableName(),
        mState(State::of(NameParserPointer(new StringParser(
                e, std::not2(Predicate<Char>(isVariableNameChar)))))) { }

bool AssignmentParserBase::isValidVariableName(
        const Environment &e, const String &s) {
    return !s.empty() && !std::isdigit(s[0], e.locale());
}

void AssignmentParserBase::parseVariableName() {
    assert(mState.index() == mState.index<NameParserPointer>());

    String name = mState.value<NameParserPointer>()->parse();

    if (isValidVariableName(environment(), name) &&
            currentCharInt() == CharTraits::to_int_type(L('='))) {
        mVariableName.emplace(std::move(name));
        ++environment().current();
    } else {
        // No variable name; The whole token is a word.
        environment().current() = mBegin;
    }
}

AssignmentParserResult AssignmentParserBase::parse() {
    if (mState.index() == mState.index<NameParserPointer>()) {
        parseVariableName();
        mState.reset(createWordParser(isTokenDelimiter));
    }

    WordPointer word = mState.value<WordParserPointer>()->parse();
    assert(word != nullptr);
    if (mVariableName.hasValue())
        return AssignmentParserResult::of(
                AssignmentPointer(new syntax::Assignment(
                        std::move(mVariableName).value(), std::move(word))));
    else
        return AssignmentParserResult::of(WordPointer(std::move(word)));
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
