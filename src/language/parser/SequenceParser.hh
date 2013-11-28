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

#ifndef INCLUDED_language_parser_SequenceParser_hh
#define INCLUDED_language_parser_SequenceParser_hh

#include "buildconfig.h"

#include <memory>
#include "language/parser/CommentSkipper.hh"
#include "language/parser/LineMode.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Sequence.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Sequence parser.
 *
 * A sequence parser parses a series of and-or lists until it encounters a
 * <dfn>stop word</dfn> like <code>fi</code> and <code>done</code>. If it is in
 * the single line mode, it stops parsing after the first unquoted newline that
 * is not part of an and-or list. (The newline must be skipped over so that all
 * pending here documents are processed before the sequence parser stops
 * parsing.)
 */
class SequenceParser final :
        public Parser<std::unique_ptr<syntax::Sequence>>,
        protected ParserBase {

public:

    using AndOrListParserPointer =
            std::unique_ptr<Parser<std::unique_ptr<syntax::AndOrList>>>;

    /**
     * A function of this type is called to create and-or list parsers while
     * parsing the sequence. The function is called each time the sequence
     * parser starts parsing an and-or list contained in the sequence.
     *
     * The function must return a new and-or list parser that should be used to
     * parse the and-or list starting from the current position of the
     * environment. If there is no valid and-or list at the current position,
     * the returned parser should return null.
     *
     * The function may peek a token at the current position to determine the
     * type of the and-or list parser it creates. The function may remove line
     * continuations from the token, but may not modify the environment
     * otherwise.
     */
    using AndOrListParserCreator =
            std::function<AndOrListParserPointer(Environment &)>;

private:

    /** Not null until parsing finishes. */
    std::unique_ptr<syntax::Sequence> mSequence;

    /** Never null. */
    AndOrListParserCreator mCreateAndOrListParser;

    /** Null while skipping whitespaces. */
    AndOrListParserPointer mAndOrListParser;

    /** Skips whitespaces between and-or lists. */
    CommentSkipper mWhitespaceSkipper;

public:

    /**
     * Creates an and-or list parser.
     * @param e An environment.
     * @param pc (Must not be null) An and-or list parser creator function.
     * @param lm A line mode.
     */
    SequenceParser(Environment &e, AndOrListParserCreator &&pc, LineMode lm);

    SequenceParser(const SequenceParser &) = delete;
    SequenceParser(SequenceParser &&) = default;
    SequenceParser& operator=(const SequenceParser &) = delete;
    SequenceParser& operator=(SequenceParser &&) = delete;
    ~SequenceParser() override = default;

private:

    bool skipNewline();

public:

    /**
     * Parses the sequence. Returns null on a parse error. Even if non-null,
     * the result may not contain any and-or lists, so the caller should check
     * if the result is non-empty.
     *
     * The environment's current position is advanced past the parsed sequence.
     *
     * If this function returns a result without throwing, the internal state
     * of this parser is no longer valid and this function must never be called
     * again.
     *
     * If more source is needed to finish parsing the sequence, this function
     * throws NeedMoreSource. In this case, the caller should set the EOF flag
     * or append to the source and then call this function again.
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    std::unique_ptr<syntax::Sequence> parse() override;

}; // class SequenceParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SequenceParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
