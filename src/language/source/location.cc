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
#include "location.hh"

#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

namespace {

bool equal(const location *l, const location *r) noexcept {
    if (l == nullptr)
        return r == nullptr;
    return r != nullptr && *l == *r;
}

} // namespace

line_location::line_location(
        std::shared_ptr<const location> &&parent, // may be null
        std::shared_ptr<const class origin> &&origin, // must never be null
        std::size_t line) :
        m_parent(std::move(parent)),
        m_origin(std::move(origin)),
        m_line(line) {
    if (m_origin == nullptr)
        throw std::invalid_argument("null origin");
}

bool operator==(const line_location &l, const line_location &r) noexcept {
    return l.line() == r.line() &&
            &l.origin() == &r.origin() &&
            equal(l.parent(), r.parent());
}

location::location(
        std::shared_ptr<const location> &&parent, // may be null
        std::shared_ptr<const class origin> &&origin, // must never be null
        std::size_t line,
        std::size_t column) :
        line_location(std::move(parent), std::move(origin), line),
        m_column(column) { }

bool operator==(const location &l, const location &r) noexcept {
    return l.column() == r.column() &&
            static_cast<const line_location &>(l) ==
            static_cast<const line_location &>(r);
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
