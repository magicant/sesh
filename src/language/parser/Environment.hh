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

#ifndef INCLUDED_language_parser_Environment_hh
#define INCLUDED_language_parser_Environment_hh

#include "common/ErrorLevel.hh"
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

public:

    Environment() = default;
    Environment(const Environment &) = default;
    Environment(Environment &&) = default;
    Environment &operator=(const Environment &) = default;
    Environment &operator=(Environment &&) = default;
    virtual ~Environment() = default;

    /** Reference to the current position in the source buffer. */
    virtual source::SourceBuffer::ConstIterator &current() noexcept = 0;

    /** Reference to the (current) end of the source buffer. */
    virtual const source::SourceBuffer::ConstIterator &end() const noexcept
            = 0;

    /**
     * Whether the end of file has been reached. If true, the parser must not
     * request further source buffer input. Otherwise, the parser may do so by
     * throwing NeedMoreSource when further input is needed to finish parsing.
     */
    virtual bool isEof() const noexcept = 0;

    /**
     * Checks whether there is a backslash-newline pair (or a line
     * continuation) at the specified iterator position and if so removes it
     * from the source buffer, invalidating all iterators thereafter.
     *
     * @return true if the line continuation was removed; false if nothing
     * changed.
     * @throw NeedMoreSource when the source is insufficient to determine if
     * there is a line continuation.
     */
    virtual bool removeLineContinuation(
            const source::SourceBuffer::ConstIterator &) = 0;

    /**
     * Performs alias substitution on the specified range of the source buffer.
     * Returns true if the range was actually substituted, in which case all
     * iterators after the begin are invalidated. Returns false if nothing
     * changed.
     */
    virtual bool substituteAlias(
            const source::SourceBuffer::ConstIterator &begin,
            const source::SourceBuffer::ConstIterator &end) = 0;

    /** Add a message to report an syntax error or issue a warning. */
    virtual void addDiagnosticMessage(
            const source::SourceBuffer::ConstIterator &position,
            common::String &&message,
            common::ErrorLevel) = 0;

};

/**
 * Returns the character pointed to by the argument iterator if the iterator is
 * before the end of the buffer. If the iterator is at the end:
 *  - EOF is returned if the environment says EOF has been reached;
 *  - NeedMoreSource is thrown otherwise.
 */
common::CharTraits::int_type dereference(
        const Environment &,
        const source::SourceBuffer::ConstIterator &);

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Environment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
