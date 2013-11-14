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

#ifndef INCLUDED_language_parser_AssignmentParserBase_hh
#define INCLUDED_language_parser_AssignmentParserBase_hh

#include "buildconfig.h"

#include <memory>
#include "common/Maybe.hh"
#include "common/String.hh"
#include "common/Variant.hh"
#include "language/parser/AssignmentParserResult.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Assignment parser. This is an abstract class that implements most part of
 * the parser. A concrete subclass must provide a factory method that creates
 * parsers used by this parser.
 */
class AssignmentParserBase :
        public Parser<AssignmentParserResult>, protected ParserBase {

protected:

    using WordParserPointer =
            std::unique_ptr<Parser<std::unique_ptr<syntax::Word>>>;

private:

    using NameParserPointer = std::unique_ptr<StringParser>;
    using State = common::Variant<NameParserPointer, WordParserPointer>;

    source::SourceBuffer::ConstIterator mBegin;
    common::Maybe<common::String> mVariableName;
    State mState;

public:

    AssignmentParserBase(Environment &);
    AssignmentParserBase(const AssignmentParserBase &) = delete;
    AssignmentParserBase(AssignmentParserBase &&) = default;
    AssignmentParserBase &operator=(const AssignmentParserBase &) = delete;
    AssignmentParserBase &operator=(AssignmentParserBase &&) = delete;
    ~AssignmentParserBase() override = default;

    using AssignmentPointer = std::unique_ptr<syntax::Assignment>;
    using WordPointer = std::unique_ptr<syntax::Word>;

private:

    /**
     * Determines if the argument string is a valid variable name, assuming the
     * string is the parse result of the name parser.
     */
    static bool isValidVariableName(
            const Environment &, const common::String &);

    /**
     * Creates a new word parser that works in the same environment as this.
     * @return non-null pointer to a new word parser.
     */
    virtual WordParserPointer createWordParser(
            Predicate<common::Char> &&isDelimiter) const = 0;

    void parseVariableName();

public:

    /**
     * Parses an assignment or word.
     *
     * If an assignment or word was parsed, it is returned in a variant. The
     * resultant word may be empty, so the caller should check the emptiness
     * for validation. The environment's current iterator position is updated
     * so that it points to the character past the parsed word.
     *
     * If this function returns without throwing, the internal state of this
     * parser is no longer valid and this function must never be called again.
     *
     * If more source is needed to finish parsing the assignment or word, this
     * function throws NeedMoreSource. In this case, the caller should set the
     * EOF flag or append to the source and then call this function again.
     *
     * @throws NeedMoreSource when more source is needed to finish parsing.
     */
    AssignmentParserResult parse() final override;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
