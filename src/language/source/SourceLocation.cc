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

#include "SourceLocation.hh"
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

SourceLocation::SourceLocation(
        const SourceLineLocation &nameAndLine, std::size_t column) :
        mNameAndLine(nameAndLine), mColumn(column) { }

SourceLocation::SourceLocation(
        SourceLineLocation &&nameAndLine, std::size_t column) :
        mNameAndLine(std::move(nameAndLine)), mColumn(column) { }

SourceLocation::SourceLocation(
        std::shared_ptr<const std::wstring> &name,
        std::size_t line,
        std::size_t column) :
        mNameAndLine(name, line),
        mColumn(column) { }

SourceLocation::SourceLocation(
        std::shared_ptr<const std::wstring> &&name,
        std::size_t line,
        std::size_t column) :
        mNameAndLine(std::move(name), line),
        mColumn(column) { }

bool operator==(const SourceLocation &l, const SourceLocation &r) {
    return l.nameAndLine() == r.nameAndLine() && l.column() == r.column();
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
