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

#ifndef INCLUDED_language_parser_ParserBase_hh
#define INCLUDED_language_parser_ParserBase_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/source/SourceBuffer.hh"

namespace sesh {
namespace language {
namespace parser {

/** Fundamental part of parser implementation. */
class ParserBase {

private:

    Environment &mEnvironment;

protected:

    using CharInt = sesh::common::CharTraits::int_type;
    using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

    explicit ParserBase(Environment &e) noexcept : mEnvironment(e) { }
    ParserBase(const ParserBase &) = default;
    ParserBase(ParserBase &&) = default;
    ParserBase &operator=(const ParserBase &) = delete;
    ParserBase &operator=(ParserBase &&) = delete;
    ~ParserBase() = default;

    Environment &environment() const noexcept { return mEnvironment; }

    /**
     * Returns the character pointed to by the argument iterator if the
     * iterator is before the end of the buffer. If the iterator is at the end:
     *  - EOF is returned if the environment says EOF has been reached; or
     *  - NeedMoreSource is thrown otherwise.
     */
    CharInt dereference(const Iterator &i) const {
        return parser::dereference(environment(), i);
    }

    CharInt currentCharInt() const {
        return dereference(environment().current());
    }

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_ParserBase_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
