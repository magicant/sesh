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

#ifndef INCLUDED_language_parser_Environment_hh
#define INCLUDED_language_parser_Environment_hh

#include "buildconfig.h"

#include <locale>
#include "common/Char.hh"
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "language/source/SourceBuffer.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parser environment is an abstract interface that provides functionalities
 * required by parsers. An environment is associated with a source buffer the
 * parser operates on.
 */
class Environment {

protected:

    ~Environment() = default;

public:

    using Size = common::String::size_type;

    /** Shared pointer to the underlying source buffer. */
    virtual const source::SourceBuffer &sourceBuffer() const noexcept = 0;

    /** Current length (end position) of the source buffer. */
    virtual Size length() const noexcept = 0;

    /** Character at the given position in the source buffer. */
    virtual common::Char at(Size) const = 0;

    /** Current position in the source buffer. */
    virtual Size position() const noexcept = 0;

    /** Sets the current position (&lt;= {@link #length}). */
    virtual void setPosition(Size) = 0;

    /**
     * Whether the end of file has been reached. If true, the parser must not
     * request further source buffer input. Otherwise, the parser may do so by
     * throwing NeedMoreSource when further input is needed to finish parsing.
     */
    virtual bool isEof() const noexcept = 0;

    /**
     * Checks whether there is a backslash-newline pair (or a line
     * continuation) at the specified position and if so removes it from the
     * source buffer.
     *
     * @return true if the line continuation was removed; false if nothing
     * changed.
     * @throw NeedMoreSource when the source is insufficient to determine if
     * there is a line continuation.
     */
    virtual bool removeLineContinuation(Size) = 0;

    /**
     * Performs alias substitution on the specified range of the source buffer.
     * Returns true if the range was actually substituted, in which case all
     * iterators after the begin are invalidated. Returns false if nothing
     * changed.
     */
    virtual bool substituteAlias(Size begin, Size end) = 0;

    /** Add a message to report an syntax error or issue a warning. */
    virtual void addDiagnosticMessage(
            Size position,
            common::Message<> &&message,
            common::ErrorLevel) = 0;

    /** Returns the locale that may affect the parsing process. */
    virtual const std::locale &locale() const noexcept = 0;

}; // class Environment

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Environment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
