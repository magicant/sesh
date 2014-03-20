/* Copyright (C) 2014 WATANABE Yuki
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

#include "buildconfig.h"
#include "SourceEnvironment.hh"

#include <cassert>
#include "common/Char.hh"
#include "language/source/Buffer.hh"

using sesh::common::Char;
using sesh::language::source::Buffer;

namespace sesh {
namespace language {
namespace parser {

SourceEnvironment::SourceEnvironment() :
        mBuffer(Buffer::create()), mPosition(), mLength(mBuffer->length()) { }

const Buffer &SourceEnvironment::buffer() const noexcept {
    return *mBuffer;
}

auto SourceEnvironment::length() const noexcept -> Size {
    return mLength;
}

Char SourceEnvironment::at(Size position) const {
    assert(position < length());
    return buffer()[position];
}

auto SourceEnvironment::position() const noexcept -> Size {
    return mPosition;
}

void SourceEnvironment::setPosition(Size newPosition) {
    assert(newPosition <= length());
    mPosition = newPosition;
}

bool SourceEnvironment::isEof() const noexcept {
    throw "unexpected isEof";
}

bool SourceEnvironment::removeLineContinuation(Size) {
    throw "unexpected removeLineContinuation";
}

bool SourceEnvironment::substituteAlias(Size, Size) {
    throw "unexpected substituteAlias";
}

void SourceEnvironment::addDiagnosticMessage(
        Size, common::Message<> &&, common::ErrorLevel) {
    throw "unexpected addDiagnosticMessage";
}

const std::locale &SourceEnvironment::locale() const noexcept {
    throw "unexpected locale";
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
