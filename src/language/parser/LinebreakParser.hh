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

#ifndef INCLUDED_language_parser_LinebreakParser_hh
#define INCLUDED_language_parser_LinebreakParser_hh

#include "buildconfig.h"

#include "common/Empty.hh"
#include "language/parser/BlankCommentNewlineParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a sequence of blank characters, comments, and newline characters.
 * This parser always succeeds.
 *
 * The name "linebreak" comes from the shell grammar definition in POSIX.
 */
class LinebreakParser : public Parser<common::Empty> {

private:

    BlankCommentNewlineParser mInnerParser;

    common::Empty mResult;

public:

    explicit LinebreakParser(Environment &);

private:

    bool innerParserProceeds();

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class LinebreakParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_LinebreakParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
