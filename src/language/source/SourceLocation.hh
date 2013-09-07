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

#ifndef INCLUDED_language_source_SourceLocation_hh
#define INCLUDED_language_source_SourceLocation_hh

#include <cstddef>
#include <memory>
#include "common/String.hh"
#include "language/source/SourceLineLocation.hh"

namespace sesh {
namespace language {
namespace source {

/** A source location is a location in a shell script source file. */
class SourceLocation {

private:

    SourceLineLocation mNameAndLine;
    /** Counted from 0. */
    std::size_t mColumn;

public:

    SourceLocation(const SourceLineLocation &nameAndLine, std::size_t column);
    SourceLocation(SourceLineLocation &&nameAndLine, std::size_t column);

    SourceLocation(
            const std::shared_ptr<const common::String> &name,
            std::size_t line,
            std::size_t column);
    SourceLocation(
            std::shared_ptr<const common::String> &&name,
            std::size_t line,
            std::size_t column);

    SourceLocation(const SourceLocation &) = default;
    SourceLocation(SourceLocation &&) = default;
    SourceLocation &operator=(const SourceLocation &) = default;
    SourceLocation &operator=(SourceLocation &&) = default;
    ~SourceLocation() = default;

    const SourceLineLocation &nameAndLine() const noexcept {
        return mNameAndLine;
    }
    const common::String &name() const noexcept { return mNameAndLine.name(); }
    std::size_t line() const noexcept { return mNameAndLine.line(); }
    std::size_t column() const noexcept { return mColumn; }

}; // class SourceLocation

bool operator==(const SourceLocation &l, const SourceLocation &r);

inline bool operator!=(const SourceLocation &l, const SourceLocation &r) {
    return !(l == r);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_SourceLocation_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
