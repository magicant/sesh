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

#ifndef INCLUDED_language_parser_SimpleCommandParserBase_hh
#define INCLUDED_language_parser_SimpleCommandParserBase_hh

#include "buildconfig.h"

#include <memory>
#include "common/Variant.hh"
#include "language/parser/AssignmentParserResult.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parser for a simple command. This is an abstract class that implements most
 * part of the parser. A concrete subclass must provide factory methods that
 * create parsers used by this parser.
 */
class SimpleCommandParserBase :
        public Parser<std::unique_ptr<syntax::Command>>, protected ParserBase {

protected:

    using AssignmentPointer = std::unique_ptr<syntax::Assignment>;
    using WordPointer = std::unique_ptr<syntax::Word>;

    using AssignmentParserPointer =
            std::unique_ptr<Parser<AssignmentParserResult>>;
    using WordParserPointer = std::unique_ptr<Parser<WordPointer>>;

private:

    // TODO redirection
    using State = common::Variant<
            CommentSkipper, AssignmentParserPointer, WordParserPointer>;

    State mState;

    std::unique_ptr<syntax::SimpleCommand> mCommand;

public:

    explicit SimpleCommandParserBase(Environment &) noexcept;
    SimpleCommandParserBase(const SimpleCommandParserBase &) = delete;
    SimpleCommandParserBase(SimpleCommandParserBase &&) = default;
    SimpleCommandParserBase &operator=(const SimpleCommandParserBase &) =
            delete;
    SimpleCommandParserBase &operator=(SimpleCommandParserBase &&) = delete;
    ~SimpleCommandParserBase() override = default;

private:

    /**
     * Creates a new assignment parser that works in the same environment as
     * this.
     * @return non-null pointer to a new assignment parser.
     */
    virtual AssignmentParserPointer createAssignmentParser() const = 0;

    /**
     * Creates a new word parser that works in the same environment as this.
     * @return non-null pointer to a new word parser.
     */
    virtual WordParserPointer createWordParser(
            Predicate<common::Char> &&isDelimiter) const = 0;

    void switchToCommentSkipper();
    void switchToAssignmentOrWordParser();

    class Processor;

public:

    /** Same as {@link #parse()}, but returns SimpleCommand type. */
    std::unique_ptr<syntax::SimpleCommand> parseSimpleCommand();

    std::unique_ptr<syntax::Command> parse() final override;

}; // class SimpleCommandParserBase

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SimpleCommandParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
