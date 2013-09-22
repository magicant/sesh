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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#include "BasicEnvironment.hh"
#include <utility>
#include "language/parser/NeedMoreSource.hh"
#include "language/source/LineContinuedSource.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"

using sesh::language::parser::NeedMoreSource;
using sesh::language::source::LineContinuedSource;
using sesh::language::source::Source;
using sesh::language::source::SourceBuffer;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

namespace sesh {
namespace language {
namespace parser {

BasicEnvironment::BasicEnvironment() :
        mBuffer(SourceBuffer::create()),
        mCurrent(mBuffer->begin()),
        mEnd(mBuffer->end()) { }

bool BasicEnvironment::removeLineContinuation(const Iterator &i) {
    if (i == end())
        return isEof() ? false : throw NeedMoreSource();
    if (*i != L('\\'))
        return false;

    Iterator next = i + 1;
    if (next == end())
        return isEof() ? false : throw NeedMoreSource();
    if (*next != L('\n'))
        return false;

    substituteSource([&i](Source::Pointer &&orig) -> Source::Pointer {
        return Source::Pointer(new LineContinuedSource(
                std::move(orig), i.position()));
    });
    return true;
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
