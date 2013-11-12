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

#ifndef INCLUDED_language_parser_SimpleCommandParserImpl_hh
#define INCLUDED_language_parser_SimpleCommandParserImpl_hh

#include "buildconfig.h"

#include <memory>
#include "common/Variant.hh"
#include "language/parser/CommandParser.hh"
#include "language/parser/CommentSkipper.hh"
#include "language/parser/ParserBase.hh"
#include "language/syntax/SimpleCommand.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parser for simple command.
 *
 * @tparam Types A placeholder type that specifies parser implementation types
 * this simple command parser depends on.
 */
template<typename Types>
class SimpleCommandParserImpl : protected ParserBase, public CommandParser {

private:

    using AssignmentParser = typename Types::AssignmentParser;
    using WordParser = typename Types::WordParser;

    // TODO redirection
    using State = common::Variant<
            CommentSkipper, AssignmentParser, WordParser>;

    State mState;

    std::unique_ptr<syntax::SimpleCommand> mCommand;

public:

    explicit SimpleCommandParserImpl(Environment &) noexcept;
    SimpleCommandParserImpl(const SimpleCommandParserImpl &) = delete;
    SimpleCommandParserImpl(SimpleCommandParserImpl &&) = default;
    SimpleCommandParserImpl &operator=(const SimpleCommandParserImpl &) =
            delete;
    SimpleCommandParserImpl &operator=(SimpleCommandParserImpl &&) = delete;
    // XXX: GCC bug #51629: ~SimpleCommandParserImpl() override = default;

private:

    void switchToCommentSkipper();
    void switchToAssignmentOrWordParser();

    class Processor {

    private:

        SimpleCommandParserImpl &mParser;

    public:

        Processor(SimpleCommandParserImpl &p) : mParser(p) { }

        using Result = bool;

        bool operator()(typename AssignmentParser::AssignmentPointer &&);
        bool operator()(typename AssignmentParser::WordPointer &&);

        bool operator()(CommentSkipper &);
        bool operator()(AssignmentParser &);
        bool operator()(WordParser &);

    };

public:

    /** Same as {@link #parse()}, but returns SimpleCommand type. */
    std::unique_ptr<syntax::SimpleCommand> parseSimpleCommand();

    std::unique_ptr<syntax::Command> parse() override;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SimpleCommandParserImpl_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
