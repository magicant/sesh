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

#ifndef INCLUDED_language_parser_TrieParser_hh
#define INCLUDED_language_parser_TrieParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "common/Trie.hh"
#include "language/parser/CharParser.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/NormalParser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A trie parser uses a trie to find an accepted character sequence. The
 * longest match from the keys in the trie is consumed by the parser and the
 * value associated with the matched key is returned.
 */
template<typename Result>
class TrieParser : public NormalParser<Result> {

public:

    using Trie = const common::Trie<common::Char, Result>;
    using Traverser = typename Trie::template ConstTraverser<>;

private:

    std::shared_ptr<Trie> mTrie;
    Traverser mCurrentNode;
    CharParser mCharParser;

public:

    /**
     * Constructs a new trie parser.
     * @param e environment.
     * @param trie non-null shared pointer to the trie used in parsing.
     */
    TrieParser(
            Environment &e,
            std::shared_ptr<Trie> &&trie,
            LineContinuationTreatment lct = LineContinuationTreatment::REMOVE)
            noexcept :
            NormalParser<Result>(e),
            mTrie(std::move(trie)),
            mCurrentNode(mTrie->traverserBegin()),
            mCharParser(
                    e,
                    [this](const Environment &, common::Char c) -> bool {
                        return mCurrentNode.down(c);
                    },
                    lct) { }

private:

    void parseImpl() final override {
        // find longest match
        while (mCharParser.parse().hasValue())
            mCharParser.reset();

        // rewind to node with value
        while (!mCurrentNode->hasValue())
            if (mCurrentNode.up() == 0)
                return;

        this->environment().setPosition(
                this->begin() + mCurrentNode.pathString().length());
        this->result().emplace(mCurrentNode->value());
    }

    void resetImpl() noexcept override {
        mCurrentNode = mTrie->traverserBegin();
        mCharParser.reset();
        NormalParser<Result>::resetImpl();
    }

}; // template<typename Result> class TrieParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_TrieParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
