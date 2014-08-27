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
#include <utility>
#include "common/Message.hh"
#include "language/source/Origin.hh"

namespace sesh {
namespace language {
namespace source {

class Location;

/** A line location is a pair of a source origin and a line number. */
class LineLocation {

private:

    /** May be null. */
    std::shared_ptr<const Location> mParent;
    /** Non-null. */
    std::shared_ptr<const Origin> mOrigin;
    /** Counted from 0. */
    std::size_t mLine;

public:

    LineLocation(
            std::shared_ptr<const Location> &&parent, // may be null
            std::shared_ptr<const Origin> &&origin, // must never be null
            std::size_t line);

    LineLocation(const LineLocation &) = default;
    LineLocation(LineLocation &&) = default;
    LineLocation &operator=(const LineLocation &) = default;
    LineLocation &operator=(LineLocation &&) = default;
    ~LineLocation() = default;

    const Location *parent() const noexcept { return mParent.get(); }
    const Origin &origin() const noexcept { return *mOrigin; }
    const std::size_t &line() const noexcept { return mLine; }

};

/**
 * Compares two line locations. The parents and origins are compared by their
 * addresses.
 */
bool operator==(const LineLocation &l, const LineLocation &r) noexcept;

inline bool operator!=(const LineLocation &l, const LineLocation &r) noexcept {
    return !(l == r);
}

/** A location points to a character position in a shell script source file. */
class Location : public LineLocation {

private:

    /** Counted from 0. */
    std::size_t mColumn;

public:

    Location(const LineLocation &lineLocation, std::size_t column) noexcept :
            LineLocation(lineLocation), mColumn(column) { }

    Location(LineLocation &&lineLocation, std::size_t column) noexcept :
            LineLocation(std::move(lineLocation)), mColumn(column) { }

    Location(
            std::shared_ptr<const Location> &&parent, // may be null
            std::shared_ptr<const Origin> &&origin, // must never be null
            std::size_t line,
            std::size_t column);

    Location(const Location &) = default;
    Location(Location &&) = default;
    Location &operator=(const Location &) = default;
    Location &operator=(Location &&) = default;
    ~Location() = default;

    std::size_t column() const noexcept { return mColumn; }

}; // class Location

bool operator==(const Location &l, const Location &r) noexcept;

inline bool operator!=(const Location &l, const Location &r) noexcept {
    return !(l == r);
}

bool operator==(const LineLocation &, const Location &) = delete;
bool operator!=(const LineLocation &, const Location &) = delete;
bool operator==(const Location &, const LineLocation &) = delete;
bool operator!=(const Location &, const LineLocation &) = delete;

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_Location_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
