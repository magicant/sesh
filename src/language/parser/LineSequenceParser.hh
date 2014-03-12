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

#ifndef INCLUDED_language_parser_LineSequenceParser_hh
#define INCLUDED_language_parser_LineSequenceParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "language/parser/OperatorParser.hh"
#include "language/parser/Parser.hh"
#include "language/parser/SequenceParserResult.hh"
#include "language/syntax/Sequence.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parses a line as a sequence. The parsed sequence must be followed by a
 * newline or reach the end-of-file. The resultant sequence may contain no
 * and-or lists if the line is empty.
 *
 * This parser always succeeds. It stops after the first newline delimiter that
 * is not part of the sequence or at the end-of-file.
 */
class LineSequenceParser : public Parser<syntax::Sequence> {

public:

    using SequenceParserPointer =
            std::unique_ptr<Parser<SequenceParserResult>>;
    using NewlineParserPointer = std::unique_ptr<Parser<common::Char>>;

private:

    SequenceParserPointer mSequenceParser;
    NewlineParserPointer mNewlineParser;
    OperatorParser mOperatorParser;

public:

    LineSequenceParser(SequenceParserPointer &&, NewlineParserPointer &&)
            noexcept;

private:

    void parseNonNewlineTrailer();

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class LineSequenceParesr

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_LineSequenceParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
