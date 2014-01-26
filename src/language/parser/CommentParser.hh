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

#ifndef INCLUDED_language_parser_CommentParser_hh
#define INCLUDED_language_parser_CommentParser_hh

#include "buildconfig.h"

#include "common/Maybe.hh"
#include "common/String.hh"
#include "language/parser/CharFilter.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NormalParser.hh"
#include "language/parser/StringParser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A comment parser parses a comment, starting from the hash sign ('#') up to
 * (but not including) a newline delimiter. The whole comment string is
 * returned as the result. This parser fails if no hash sign was found at the
 * beginning position.
 */
class CommentParser : public NormalParser<common::String> {

private:

    CharFilter mHashFilter;
    StringParser mContentParser;

public:

    /**
     * Constructs a new comment parser.
     *
     * @param lct Specifies line continuation treatment at the beginning. Does
     * not affect parsing of the comment body.
     */
    explicit CommentParser(
            Environment &,
            LineContinuationTreatment lct = LineContinuationTreatment::REMOVE);

private:

    void parseImpl() override;

    void resetImpl() noexcept override;

}; // class CommentParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CommentParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
