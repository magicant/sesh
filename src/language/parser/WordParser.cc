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

#include "buildconfig.h"
#include "WordParser.hh"

#include <cassert>
#include <stdexcept>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::language::syntax::Word;

namespace sesh {
namespace language {
namespace parser {

WordParser::WordParser(
        Environment &e,
        ComponentParserCreator &&cpc) :
        Parser(),
        ParserBase(e),
        mCreateComponentParser(std::move(cpc)),
        mWord(new Word),
        mComponentParser(nullptr) {
    assert(mCreateComponentParser != nullptr);
}

bool WordParser::parseComponent() {
    if (mComponentParser == nullptr) {
        mComponentParser = mCreateComponentParser(environment());
        if (mComponentParser == nullptr)
            return false;
    }
    auto component = mComponentParser->parse();
    mComponentParser = nullptr;
    if (component == nullptr)
        return false;
    mWord->components().push_back(std::move(component));
    return true;
}

std::unique_ptr<Word> WordParser::parse() {
    if (mWord == nullptr)
        throw std::logic_error("invalid parser state");

    while (parseComponent()) { }
    return std::move(mWord);
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
