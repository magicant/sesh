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

#ifndef INCLUDED_language_parser_SequenceParser_hh
#define INCLUDED_language_parser_SequenceParser_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "common/Variant.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/SequenceParserResult.hh"
#include "language/parser/TokenParser.hh"
#include "language/syntax/Sequence.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a sequence of and-or lists, possibly followed by a keyword that
 * signifies the end of the sequence.
 *
 * This parser always succeeds.
 *
 * This parser stops parsing when it encounters a newline character.
 */
class SequenceParser : public Parser<SequenceParserResult> {

public:

    using TokenParserPointer = std::unique_ptr<TokenParser>;
    using AndOrListPointer = syntax::Sequence::AndOrListPointer;
    using AndOrListParserPointer =
            std::unique_ptr<Parser<std::pair<AndOrListPointer, bool>>>;

private:

    using InnerParser =
            common::Variant<TokenParserPointer, AndOrListParserPointer>;

    /**
     * The token parser pointer may be null then the sequence parser is not yet
     * started. The token parser will be created on the first call to {@link
     * #parseImpl}. After the first token is parsed and interpreted, a new
     * and-or list parser adopts the token parser to parse the rest of the
     * and-or list.
     */
    InnerParser mInnerParser;

    SequenceParserResult mResult;

public:

    explicit SequenceParser(Environment &) noexcept;

private:

    virtual TokenParserPointer createTokenParser() const = 0;

    virtual AndOrListParserPointer createAndOrListParser(TokenParserPointer &&)
            const = 0;

    class InnerParserProcessor;

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class SequenceParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SequenceParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
