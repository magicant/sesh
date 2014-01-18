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

#ifndef INCLUDED_language_parser_CharFilter_hh
#define INCLUDED_language_parser_CharFilter_hh

#include "buildconfig.h"

#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/Predicate.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * CharFilter checks if the current character satisfies a given predicate. This
 * parser does not move the current position.
 */
class CharFilter : public NormalParser<common::CharTraits::int_type> {

private:

    Predicate<common::Char> mIsAcceptableChar;
    LineContinuationTreatment mLineContinuationTreatment;

public:

    CharFilter(
            Environment &,
            Predicate<CharInt> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

    using NormalParser::reset;

    void reset(
            Predicate<CharInt> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

private:

    void parseImpl() override;

}; // class CharFilter

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CharFilter_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
