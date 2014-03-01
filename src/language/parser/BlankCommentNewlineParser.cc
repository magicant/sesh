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
#include "BlankCommentNewlineParser.hh"

#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"

using sesh::common::Char;
using sesh::common::Maybe;
using sesh::common::String;

namespace sesh {
namespace language {
namespace parser {

BlankCommentNewlineParser::BlankCommentNewlineParser(Environment &e) :
        Parser<String>(e),
        mBlankAndCommentParser(e),
        mNewlineParser(e) { }

void BlankCommentNewlineParser::parseImpl() {
    String &result = mBlankAndCommentParser.parse().value();
    if (Maybe<Char> &newline = mNewlineParser.parse())
        result.push_back(*newline);
}

void BlankCommentNewlineParser::resetImpl() noexcept {
    mBlankAndCommentParser.reset();
    mNewlineParser.reset();
}

Maybe<String> &BlankCommentNewlineParser::result() {
    return mBlankAndCommentParser.parse();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */