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

#ifndef INCLUDED_language_parser_Parser_hh
#define INCLUDED_language_parser_Parser_hh

#include "buildconfig.h"

#include "common/Maybe.hh"
#include "language/parser/ParserBase.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Fundamental part of parser implementation. This abstract class template
 * provides functionalities that are common to all parser classes but dependent
 * on the result type.
 *
 * @tparam Result_ Type of the result of this parser. The type must be
 * containable in {@link sesh::common::Maybe}.
 *
 * @see ParserBase for details
 */
template<typename Result_>
class Parser : public ParserBase {

public:

    /** Type of the result of this parser. */
    using Result = Result_;

private:

    /**
     * Returns a reference to the parse result. This method is called only when
     * the state is "finished".
     */
    virtual common::Maybe<Result> &result() = 0;

    bool isSuccessful() final override {
        return result().hasValue();
    }

public:

    using ParserBase::ParserBase;

    Parser(const Parser &) = default;
    Parser(Parser &&) = default;
    Parser &operator=(const Parser &) = default;
    Parser &operator=(Parser &&) = default;
    virtual ~Parser() noexcept = default;

    /**
     * Parses the source code according to the syntax rules of this parser.
     *
     * If the current {@link #state} is "unstarted", it is changed to "parsing"
     * and {@link ParserBase#parseImpl} is called. If it returns without
     * throwing an exception, the state is changed to "finished" and the {@link
     * result} is returned. If parseImpl throws, the exception is propagated to
     * the caller.
     *
     * If the current state is "parsing", parseImpl is called again to resume
     * parsing.
     *
     * If the current state is "finished", this method has no side effects.
     *
     * @return Reference to the result. The result maybe object is non-empty if
     * and only if the parsing was successful. The result is not
     * const-qualified, so the caller may modify it.
     *
     * @throws IncompleteParse Thrown when the source code is not enough to
     * finish parsing. When this exception is thrown, the current position of
     * the environment is left at the then position. The caller should append
     * more source code to the environment's buffer and call this method again
     * to resume parsing. The position must not be changed between the calls to
     * this method.
     *
     * @throws Reparse Thrown when the parser should be reset and restarted
     * because the source code has been changed (typically due to alias
     * substitution). When this exception is thrown, the parser state is {@link
     * #reset} to "unstarted" and the current position is restored to {@link
     * ParserBase#begin}.
     */
    common::Maybe<Result> &parse() {
        parseIfNotFinished();
        return result();
    }

}; // template<typename Result_> class Parser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Parser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
