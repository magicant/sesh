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
#include "buffer.hh"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

buffer::const_iterator::const_iterator(
        const std::shared_ptr<const class buffer> &buffer, size_type position)
        noexcept :
        m_buffer(buffer), m_position(position) { }

buffer::const_iterator::const_iterator(
        std::shared_ptr<const class buffer> &&buffer, size_type position)
        noexcept :
        m_buffer(std::move(buffer)), m_position(position) { }

auto buffer::create() -> std::shared_ptr<buffer> {
    return std::make_shared<buffer>();
}

auto buffer::length() const noexcept -> size_type {
    if (m_source == nullptr)
        return 0;
    return m_source->length();
}

auto buffer::at(size_type position) const -> const_reference {
    if (m_source == nullptr)
        throw std::out_of_range("empty source");
    return m_source->at(position);
}

auto buffer::operator[](size_type position) const -> const_reference {
    if (m_source == nullptr)
        return L("")[0];
    return (*m_source)[position];
}

auto buffer::cbegin() const noexcept -> const_iterator {
    return const_iterator(shared_from_this(), 0);
}

auto buffer::cend() const noexcept -> const_iterator {
    return const_iterator(shared_from_this(), length());
}

Location buffer::location(size_type position) const {
    return m_source->location(position);
}

buffer::string_type to_string(
        const buffer::const_iterator &begin,
        const buffer::const_iterator &end) {
    buffer::string_type s;
    s.insert(s.end(), begin, end);
    return s;
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
