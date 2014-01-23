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

#ifndef INCLUDED_language_parser_AssignmentParserImpl_hh
#define INCLUDED_language_parser_AssignmentParserImpl_hh

#include "buildconfig.h"

#include "language/parser/AssignmentParser.hh"

namespace sesh {
namespace language {
namespace parser {

class AssignmentParserImpl : public AssignmentParser {

    using AssignmentParser::AssignmentParser;

    StringParserPointer createStringParser(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const override;

    CharParserPointer createCharParser(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const override;

    WordParserPointer createAssignedWordParser(
            Predicate<common::Char> &&isAcceptableChar)
            const override;

}; // class AssignmentParserImpl

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParserImpl_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
