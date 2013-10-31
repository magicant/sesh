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

#ifndef INCLUDED_language_parser_AssignmentParserImpl_hh
#define INCLUDED_language_parser_AssignmentParserImpl_hh

#include "buildconfig.h"

#include <memory>
#include "common/Maybe.hh"
#include "common/Variant.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Assignment parser. It parses a token as an assignment or word.
 *
 * @tparam Types A placeholder type that specifies parser types that are used
 * by the assignment parser.
 *
 * @see syntax::Assignment
 * @see syntax::Word
 */
template<typename Types>
class AssignmentParserImpl : public Parser {

private:

    using NameParser = typename Types::StringParser;
    using WordParser = typename Types::WordParser;
    using State = common::Variant<NameParser, WordParser>;

    source::SourceBuffer::ConstIterator mBegin;
    common::Maybe<common::String> mVariableName;
    State mState;

public:

    AssignmentParserImpl(Environment &);
    AssignmentParserImpl(const AssignmentParserImpl &) = delete;
    AssignmentParserImpl(AssignmentParserImpl &&) = default;
    AssignmentParserImpl &operator=(const AssignmentParserImpl &) = delete;
    AssignmentParserImpl &operator=(AssignmentParserImpl &&) = delete;
    // XXX: GCC bug #51629: ~AssignmentParserImpl() = default;

    using AssignmentPointer = std::unique_ptr<syntax::Assignment>;
    using WordPointer = std::unique_ptr<syntax::Word>;
    using Result = common::Variant<AssignmentPointer, WordPointer>;

private:

    /**
     * Determines if the argument string is a valid variable name, assuming the
     * string is the parse result of the name parser.
     */
    static bool isValidVariableName(
            const Environment &, const common::String &);

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
     * @throws NeedMoreSource (see above) @throws std::logic_error when the
     * state of the parser is invalid.
     */
    Result parse();

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParserImpl_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
