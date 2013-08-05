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

#include "common.hh"
#include "SourceLocation.hh"
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {

SourceLocation::SourceLocation(
        std::shared_ptr<const std::wstring> &name,
        std::size_t line,
        std::size_t column) :
        mName(name),
        mLine(line),
        mColumn(column) {
    if (mName == nullptr)
        throw std::invalid_argument("null name");
}

SourceLocation::SourceLocation(
        std::shared_ptr<const std::wstring> &&name,
        std::size_t line,
        std::size_t column) :
        mName(std::move(name)),
        mLine(line),
        mColumn(column) {
    if (mName == nullptr)
        throw std::invalid_argument("null name");
}

} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79: */
