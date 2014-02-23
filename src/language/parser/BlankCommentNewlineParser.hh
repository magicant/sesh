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

#ifndef INCLUDED_language_parser_BlankCommentNewlineParser_hh
#define INCLUDED_language_parser_BlankCommentNewlineParser_hh

#include "buildconfig.h"

#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/BlankAndCommentParser.hh"
#include "language/parser/NewlineParser.hh"
#include "language/parser/Parser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * The blank-comment-newline parser is a combination of {@link
 * BlankAndCommentParser} and {@link NewlineParser}. This parser always
 * succeeds, resulting the skipped string which may possibly be empty.
 *
 * This parser parses a sequence of blank characters, a comment, and a newline
 * character, in this order, all of which are optional.
 */
class BlankCommentNewlineParser : public Parser<common::String> {

private:

    BlankAndCommentParser mBlankAndCommentParser;
    NewlineParser mNewlineParser;

public:

    explicit BlankCommentNewlineParser(Environment &);

private:

    void parseImpl() final override;

    void resetImpl() noexcept final override;

    common::Maybe<common::String> &result() noexcept final override;

}; // class BlankCommentNewlineParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_BlankCommentNewlineParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
