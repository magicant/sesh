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

#ifndef INCLUDED_language_source_Location_hh
#define INCLUDED_language_source_Location_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include "common/String.hh"
#include "language/source/LineLocation.hh"

namespace sesh {
namespace language {
namespace source {

/** A location points to a character position in a shell script source file. */
class Location {

private:

    LineLocation mNameAndLine;
    /** Counted from 0. */
    std::size_t mColumn;

public:

    Location(const LineLocation &nameAndLine, std::size_t column);
    Location(LineLocation &&nameAndLine, std::size_t column);

    Location(
            const std::shared_ptr<const common::String> &name,
            std::size_t line,
            std::size_t column);
    Location(
            std::shared_ptr<const common::String> &&name,
            std::size_t line,
            std::size_t column);

    Location(const Location &) = default;
    Location(Location &&) = default;
    Location &operator=(const Location &) = default;
    Location &operator=(Location &&) = default;
    ~Location() = default;

    const LineLocation &nameAndLine() const noexcept {
        return mNameAndLine;
    }
    const common::String &name() const noexcept { return mNameAndLine.name(); }
    std::size_t line() const noexcept { return mNameAndLine.line(); }
    std::size_t column() const noexcept { return mColumn; }

}; // class Location

bool operator==(const Location &l, const Location &r);

inline bool operator!=(const Location &l, const Location &r) {
    return !(l == r);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_Location_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
