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
#include "AssignmentParser.hh"

#include <locale>
#include <memory>
#include <utility>
#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "common/Variant.hh"
#include "language/parser/CharPredicates.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/syntax/Assignment.hh"

using sesh::common::Char;
using sesh::common::Maybe;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::syntax::Assignment;

namespace sesh {
namespace language {
namespace parser {

namespace {

constexpr bool isEqual(const Environment &, Char c) noexcept {
    return c == L('=');
}

bool isAssignedWordChar(const Environment &e, Char c) {
    return c != L(':') && isRawStringChar(e, c);
}

} // namespace

AssignmentParser::AssignmentParser(Environment &e) :
        NormalParser(e), mNameParser(), mEqualParser(), mWordParser() { }

bool AssignmentParser::isValidName(const String &s) const {
    return !s.empty() && !std::isdigit(s.front(), environment().locale());
}

void AssignmentParser::parseImpl() {
    if (mNameParser == nullptr)
        mNameParser = createStringParser(isVariableNameChar);
    auto &maybeName = mNameParser->parse();
    if (!maybeName.hasValue())
        return;
    if (!isValidName(maybeName.value()))
        return;

    if (mEqualParser == nullptr)
        mEqualParser = createCharParser(isEqual);
    auto &maybeEqual = mEqualParser->parse();
    if (!maybeEqual.hasValue())
        return;

    if (mWordParser == nullptr)
        mWordParser = createAssignedWordParser(isAssignedWordChar);
    auto &maybeWord = mWordParser->parse();
    if (!maybeWord.hasValue())
        return;

    result().emplace(new Assignment(
            std::move(maybeName.value()), std::move(maybeWord.value())));
}

void AssignmentParser::resetImpl() noexcept {
    if (mNameParser != nullptr)
        mNameParser->reset();
    if (mEqualParser != nullptr)
        mEqualParser->reset();
    if (mWordParser != nullptr)
        mWordParser->reset();
    NormalParser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
