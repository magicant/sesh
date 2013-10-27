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
#include "RawStringParser.hh"

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/WordComponent.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::language::syntax::RawString;
using sesh::language::syntax::WordComponent;

namespace sesh {
namespace language {
namespace parser {

RawStringParser::RawStringParser(
        Environment &e,
        Predicate<Char> &&isDelimiter,
        LineContinuationTreatment lct) :
        WordComponentParser(),
        mStringParser(e, std::move(isDelimiter), lct) { }

std::unique_ptr<RawString> RawStringParser::parseRawString() {
    String value = mStringParser.parse();
    return std::unique_ptr<RawString>(new RawString(std::move(value)));
}

std::unique_ptr<WordComponent> RawStringParser::parse() {
    return parseRawString();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
