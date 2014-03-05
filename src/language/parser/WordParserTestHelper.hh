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

#ifndef INCLUDED_language_parser_WordParserTestHelper_hh
#define INCLUDED_language_parser_WordParserTestHelper_hh

#include "buildconfig.h"

#include <memory>
#include "common/String.hh"
#include "language/parser/CharPredicates.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/RawString.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A parser stub that always succeeds with a (possibly empty) word. The word
 * contains a single-character raw string if and only if the current character
 * is a normal word character.
 */
class WordParserStub : public Parser<std::unique_ptr<syntax::Word>> {

    using Parser<std::unique_ptr<syntax::Word>>::Parser;

    std::unique_ptr<syntax::Word> mResultWord;

    void parseImpl() override {
        using sesh::common::CharTraits;

        mResultWord.reset(new syntax::Word);
        result() = &mResultWord;

        auto ci = currentCharInt();
        if (CharTraits::eq_int_type(ci, CharTraits::eof()))
            return;

        auto c = CharTraits::to_char_type(ci);
        if (!isRawStringChar(environment(), c))
            return;

        mResultWord->addComponent(std::unique_ptr<syntax::WordComponent>(
                new syntax::RawString(common::String(1, c))));
        environment().setPosition(environment().position() + 1);
    }

    void resetImpl() noexcept override {
        mResultWord.reset();
        Parser<std::unique_ptr<syntax::Word>>::resetImpl();
    }

}; // class WordParserStub

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParserTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
