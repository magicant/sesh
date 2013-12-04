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
#include "WordComponentParserBase.hh"

#include <utility>
#include "common/String.hh"

using sesh::common::Char;
using sesh::common::CharTraits;

namespace sesh {
namespace language {
namespace parser {

WordComponentParserBase::WordComponentParserBase(
        Environment &e, Predicate<Char> &&isDelimiter) :
        Parser(),
        ParserBase(e),
        mIsDelimiter(std::move(isDelimiter)),
        mActualParser(nullptr) { }

void WordComponentParserBase::prepareActualParser() {
    if (mActualParser != nullptr)
        return;

    while (environment().removeLineContinuation(environment().current())) { }

    CharInt ci = dereference(environment().current());
    if (CharTraits::eq_int_type(ci, CharTraits::eof()))
        return;
    if (mIsDelimiter(environment(), CharTraits::to_char_type(ci)))
        return;
    // TODO support other types of component parsers

    mActualParser = createRawStringParser(Predicate<Char>(mIsDelimiter));
}

auto WordComponentParserBase::parse() -> ComponentPointer {
    prepareActualParser();
    if (mActualParser == nullptr)
        return nullptr;
    return mActualParser->parse();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
