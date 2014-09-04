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

bool equal(const Location *l, const Location *r) noexcept {
    if (l == nullptr)
        return r == nullptr;
    return r != nullptr && *l == *r;
}

} // namespace

LineLocation::LineLocation(
        std::shared_ptr<const Location> &&parent, // may be null
        std::shared_ptr<const Origin> &&origin, // must never be null
        std::size_t line) :
        mParent(std::move(parent)),
        mOrigin(std::move(origin)),
        mLine(line) {
    if (mOrigin == nullptr)
        throw std::invalid_argument("null origin");
}

bool operator==(const LineLocation &l, const LineLocation &r) noexcept {
    return l.line() == r.line() &&
            &l.origin() == &r.origin() &&
            equal(l.parent(), r.parent());
}

Location::Location(
        std::shared_ptr<const Location> &&parent, // may be null
        std::shared_ptr<const Origin> &&origin, // must never be null
        std::size_t line,
        std::size_t column) :
        LineLocation(std::move(parent), std::move(origin), line),
        mColumn(column) { }

bool operator==(const Location &l, const Location &r) noexcept {
    return l.column() == r.column() &&
            static_cast<const LineLocation &>(l) ==
            static_cast<const LineLocation &>(r);
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
