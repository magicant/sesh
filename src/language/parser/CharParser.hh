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

#ifndef INCLUDED_language_parser_CharParser_hh
#define INCLUDED_language_parser_CharParser_hh

#include "buildconfig.h"

#include "common/Char.hh"
#include "common/Maybe.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * CharParser parses a single character.
 *
 * The set of characters accepted by this parser is specified by a predicate.
 * Parsing succeeds if the predicate is true for the character.
 */
class CharParser : public Parser<common::Char> {

private:

    Predicate<common::Char> mIsAcceptableChar;
    LineContinuationTreatment mLineContinuationTreatment;

    common::Char mCurrentChar;

public:

    CharParser(
            Environment &,
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

    using Parser::reset;

    void reset(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

private:

    void parseImpl() override;

}; // class CharParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CharParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
