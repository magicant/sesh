/* Copyright (C) 2013-2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_AssignmentParser_hh
#define INCLUDED_language_parser_AssignmentParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/StringParser.hh"
#include "language/parser/WordComponentParser.hh"
#include "language/parser/WordParser.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * This is an abstract class that implements most part of the assignment
 * parser. A concrete subclass must provide a factory method that creates
 * parsers used by this parser.
 */
class AssignmentParser : public Parser<std::unique_ptr<syntax::Assignment>> {

protected:

    using StringParser = Parser<common::String>;
    using StringParserPointer = std::unique_ptr<StringParser>;

    using CharParser = Parser<common::Char>;
    using CharParserPointer = std::unique_ptr<CharParser>;

    using WordParser = Parser<std::unique_ptr<syntax::Word>>;
    using WordParserPointer = std::unique_ptr<WordParser>;

    using AssignmentPointer = std::unique_ptr<syntax::Assignment>;

private:

    StringParserPointer mNameParser;
    CharParserPointer mEqualParser;
    WordParserPointer mWordParser;

    AssignmentPointer mResultAssignment;

public:

    explicit AssignmentParser(Environment &);

private:

    /**
     * Creates a new string parser.
     * @return non-null pointer to a new string parser.
     */
    virtual StringParserPointer createStringParser(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const = 0;

    /**
     * Creates a new character parser.
     * @return non-null pointer to a new character parser.
     */
    virtual CharParserPointer createCharParser(
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE)
            const = 0;

    /**
     * Creates a new assigned word parser.
     * @return non-null pointer to a new assigned word parser.
     */
    virtual WordParserPointer createAssignedWordParser(
            Predicate<common::Char> &&isAcceptableChar)
            const = 0;

    bool isValidName(const common::String &) const;

    void parseImpl() final override;

    void resetImpl() noexcept final override;

}; // class AssignmentParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
