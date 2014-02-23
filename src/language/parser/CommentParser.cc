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
#include "CommentParser.hh"

#include <cassert>
#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/StringParser.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::common::Maybe;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::parser::LineContinuationTreatment;

using CharInt = CharTraits::int_type;

namespace sesh {
namespace language {
namespace parser {

namespace {

constexpr bool isHashSign(const Environment &, CharInt c) noexcept {
    return CharTraits::eq_int_type(c, CharTraits::to_int_type(L('#')));
}

constexpr bool isNotNewline(const Environment &, Char c) noexcept {
    return c != L('\n');
}

} // namespace

CommentParser::CommentParser(Environment &e, LineContinuationTreatment lct) :
        NormalParser(e),
        mHashFilter(e, isHashSign, lct),
        mContentParser(StringParser::create(
                e, isNotNewline, LineContinuationTreatment::LITERAL)) { }

void CommentParser::parseImpl() {
    if (mHashFilter.parse().hasValue())
        result() = std::move(mContentParser.parse());
}

void CommentParser::resetImpl() noexcept {
    mHashFilter.reset();
    mContentParser.reset();
    NormalParser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
