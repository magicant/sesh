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

#ifndef INCLUDED_language_parser_CommandParserBase_hh
#define INCLUDED_language_parser_CommandParserBase_hh

#include "buildconfig.h"

#include <memory>
#include "language/parser/Parser.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/Command.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Command parser. This is an abstract class that implements some part of the
 * parser. A concrete subclass must provide factory methods that create parsers
 * used by this parser.
 */
class CommandParserBase :
        public Parser<std::unique_ptr<syntax::Command>>, protected ParserBase {

protected:

    using CommandParserPointer =
            std::unique_ptr<Parser<std::unique_ptr<syntax::Command>>>;

private:

    CommandParserPointer mActualParser;

public:

    explicit CommandParserBase(Environment &) noexcept;
    CommandParserBase(const CommandParserBase &) = delete;
    CommandParserBase(CommandParserBase &&) = default;
    CommandParserBase &operator=(const CommandParserBase &) = delete;
    CommandParserBase &operator=(CommandParserBase &&) = delete;
    ~CommandParserBase() override = default;

private:

    /**
     * Creates a new simple command parser that works in the same environment
     * as this.
     * @return non-null pointer to a new assignment parser.
     */
    virtual CommandParserPointer createSimpleCommandParser() const = 0;

    void createActualParserFromKeyword();
    void createActualParser();

public:

    std::unique_ptr<syntax::Command> parse() final override;

}; // class CommandParserBase

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CommandParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
