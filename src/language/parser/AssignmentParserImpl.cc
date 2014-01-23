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
#include "AssignmentParserImpl.hh"

#include <memory>
#include <utility>
#include "common/Char.hh"
#include "language/parser/AssignedWordParser.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/StringParser.hh"
#include "language/parser/WordParser.hh"

using sesh::common::Char;

namespace sesh {
namespace language {
namespace parser {

auto AssignmentParserImpl::createStringParser(
        Predicate<Char> &&isAcceptableChar,
        LineContinuationTreatment lct)
        const -> StringParserPointer {
    using sesh::language::parser::StringParser;
    return StringParserPointer(new StringParser(StringParser::create(
            environment(), std::move(isAcceptableChar), lct)));
}

auto AssignmentParserImpl::createCharParser(
        Predicate<Char> &&isAcceptableChar,
        LineContinuationTreatment lct)
        const -> CharParserPointer {
    using sesh::language::parser::CharParser;
    return CharParserPointer(new CharParser(
            environment(), std::move(isAcceptableChar), lct));
}

auto AssignmentParserImpl::createAssignedWordParser(
        Predicate<Char> &&isAcceptableChar)
        const -> WordParserPointer {
    using sesh::language::parser::WordParser;
    return WordParserPointer(new AssignedWordParser(
            environment(),
            WordParserPointer(new WordParser(
                    environment(), std::move(isAcceptableChar)))));
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */