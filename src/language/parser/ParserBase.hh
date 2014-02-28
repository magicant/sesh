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

#ifndef INCLUDED_language_parser_ParserBase_hh
#define INCLUDED_language_parser_ParserBase_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include <utility>
#include "common/String.hh"
#include "language/parser/Environment.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Fundamental part of parser implementation. This class is meant to be derived
 * from by the {@link Parser} class. The parser base implements part of the
 * parser that does not depend on the type of the result.
 *
 * A parser has a threefold state: unstarted, parsing, and finished. When the
 * {@link Parser#parse} method is first called, the state changes from
 * "unstarted" to "parsing". If parsing finishes in one shot, the state becomes
 * "finished" before the parse method returns. Otherwise, the state remains
 * "parsing" and the parse method throws an exception. In the latter case, the
 * parse method should be called again to finish parsing.
 *
 * Parsing may finish either successfully or unsuccessfully, which is indicated
 * by the return value of {@link Parser#parse}. The parse result can be
 * accessed only after a successful parse. If successful, the parser may
 * consume some characters from the source string by incrementing the source
 * position of the environment, so that a next parser can continue parsing
 * thereafter. If parsing was unsuccessful, the position remains the same as
 * when the state was changed from "unstarted" to "parsing" (but the position
 * might be temporarily moved while parsing).
 *
 * If the parser detects an unrecoverable syntax error, it is reported to the
 * environment. Whether parsing finished successfully or not does not affect
 * whether syntax errors should be reported. Parsing should finish successfully
 * if and only if the parse method consumed some part of the source code to
 * produce the result. Syntax errors should be reported if and only if the
 * source code does not obey the rules of the shell language syntax and no
 * other parsers are expected to recover from the error. The parser may report
 * non-error diagnostic messages (i.e. warnings and notes), but they should
 * be considered unrecoverable as well.
 */
class ParserBase {

public:

    enum class State { UNSTARTED, PARSING, FINISHED, };

protected:

    using Size = Environment::Size;

    using CharInt = sesh::common::CharTraits::int_type;

private:

    std::reference_wrapper<Environment> mEnvironment;

    State mState = State::UNSTARTED;

    /** Valid only after started parsing. */
    Size mBegin;

public:

    explicit ParserBase(Environment &) noexcept;
    explicit ParserBase(Environment &&) = delete;

    Environment &environment() const noexcept { return mEnvironment; }
    State state() const noexcept { return mState; }

    /** Valid only after started parsing. */
    const Size &begin() const noexcept { return mBegin; }

protected:

    /**
     * Calls {@link sesh::language::parser::charIntAt} for the current
     * position.
     */
    CharInt currentCharInt() const;

private:

    /**
     * Performs the actual parsing process and produces the result. This method
     * is called from the {@link #parseIfNotFinished} method when the state is
     * not "finished".
     *
     * If parsing finishes, either successfully or unsuccessfully, then this
     * method must return without throwing an exception and then the state will
     * be changed to "finished".
     *
     * @throws IncompleteParse Thrown when the source code is not enough to
     * finish parsing.
     * @throws Reparse Thrown when the parser should be reset and restarted
     * because the source code has been changed.
     *
     * @see Parser#parse for more detail about exceptions that may be thrown.
     */
    virtual void parseImpl() = 0;

    /**
     * Returns whether parsing was successful or not. This method is called
     * only when the state is "finished". This method is not const-qualified
     * because it depends on the non-const {@link Parser#result} method.
     */
    virtual bool isSuccessful() = 0;

    /**
     * Clears any intermediate or finished parse results so that a next call to
     * {@link #parseImpl} will start a new parsing. This method is called from
     * {@link #reset}.
     */
    virtual void resetImpl() noexcept { }

protected:

    /**
     * Calls {@link #parseImpl} and updates the state if the state is not
     * "finished".
     */
    void parseIfNotFinished();

public:

    /**
     * Resets the parser to the "unstarted" state. Any existing parse results
     * are cleared.
     */
    void reset() noexcept;

}; // class ParserBase

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_ParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
