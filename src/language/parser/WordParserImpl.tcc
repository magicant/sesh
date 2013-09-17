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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_parser_WordParserImpl_tcc
#define INCLUDED_language_parser_WordParserImpl_tcc

#include "WordParserImpl.hh"
#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

template<typename Types>
WordParserImpl<Types>::WordParserImpl(
        Environment &e,
        Predicate<common::Char> &&isDelimiter) :
        Parser(e),
        mIsDelimiter(std::move(isDelimiter)),
        mWord(new syntax::Word),
        mCurrentComponentParser(nullptr) {
    if (mIsDelimiter == nullptr)
        mIsDelimiter = [](Environment &, common::Char) { return false; };
}

template<typename Types>
void WordParserImpl<Types>::createComponentParser() {
    if (mCurrentComponentParser != nullptr)
        return;

    environment().removeLineContinuation(environment().current());

    // TODO support other types of word component parser

    using common::CharTraits;
    CharInt ci = dereference(environment().current());
    if (CharTraits::eq_int_type(ci, CharTraits::eof()))
        return;
    if (mIsDelimiter(environment(), CharTraits::to_char_type(ci)))
        return;

    mCurrentComponentParser.reset(new typename Types::RawStringParser(
            environment(), Predicate<common::Char>(mIsDelimiter)));
}

template<typename Types>
std::unique_ptr<syntax::Word> WordParserImpl<Types>::parse() {
    if (mWord == nullptr)
        throw std::logic_error("invalid parser state");

    while (createComponentParser(), mCurrentComponentParser != nullptr) {
        auto result = mCurrentComponentParser->parse();
        mCurrentComponentParser = nullptr;

//        if (result == nullptr) // parse error?
//            return nullptr;
        mWord->components().emplace_back(std::move(result));
    }

    return std::move(mWord);
}

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_WordParserImpl_tcc

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
