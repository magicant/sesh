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

#ifndef INCLUDED_language_parser_WordParserTestHelper_hh
#define INCLUDED_language_parser_WordParserTestHelper_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "common/Char.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Skipper.hh"
#include "language/source/SourceBuffer.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

class WordParserStub : protected ParserBase {

private:

    using Iterator = source::SourceBuffer::ConstIterator;

    Iterator mBegin;
    Skipper mSkipper;

public:

    WordParserStub(Environment &e, Predicate<common::Char> &&isDelimiter) :
            ParserBase(e),
            mBegin(e.current()),
            mSkipper(e, std::move(isDelimiter)) { }

    std::unique_ptr<syntax::Word> parse() {
        mSkipper.skip();

        std::unique_ptr<syntax::Word> word(new syntax::Word);
        if (environment().current() != mBegin) {
            word->components().emplace_back(new syntax::RawString(
                    toString(mBegin, environment().current())));
        }
        return word;
    }

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParserTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
