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

#include "buildconfig.h"
#include "Buffer.hh"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

Buffer::ConstIterator::ConstIterator(
        const ConstPointer &buffer, Size position) noexcept :
        mBuffer(buffer), mPosition(position) { }

Buffer::ConstIterator::ConstIterator(
        ConstPointer &&buffer, Size position) noexcept :
        mBuffer(std::move(buffer)), mPosition(position) { }

auto Buffer::create() -> Pointer {
    return std::make_shared<Buffer>();
}

auto Buffer::length() const noexcept -> Size {
    if (mSource == nullptr)
        return 0;
    return mSource->length();
}

auto Buffer::at(Size position) const -> ConstReference {
    if (mSource == nullptr)
        throw std::out_of_range("empty source");
    return mSource->at(position);
}

auto Buffer::operator[](Size position) const -> ConstReference {
    if (mSource == nullptr)
        return L("")[0];
    return (*mSource)[position];
}

auto Buffer::cbegin() const noexcept -> ConstIterator {
    return ConstIterator(shared_from_this(), 0);
}

auto Buffer::cend() const noexcept -> ConstIterator {
    return ConstIterator(shared_from_this(), length());
}

Location Buffer::location(Size position) const {
    return mSource->location(position);
}

Buffer::String toString(
        const Buffer::ConstIterator &begin, const Buffer::ConstIterator &end) {
    Buffer::String s;
    s.insert(s.end(), begin, end);
    return s;
}

} // namespace source
} // namespace language
} // namespace sesh

namespace std {

using ConstIterator = sesh::language::source::Buffer::ConstIterator;

template<>
void swap<ConstIterator>(ConstIterator &i1, ConstIterator &i2) noexcept {
    swap(i1.bufferPointer(), i2.bufferPointer());
    swap(i1.position(), i2.position());
}

} // namespace std

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
