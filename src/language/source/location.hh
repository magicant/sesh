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

#ifndef INCLUDED_language_source_location_hh
#define INCLUDED_language_source_location_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include <utility>
#include "common/message.hh"
#include "language/source/origin.hh"

namespace sesh {
namespace language {
namespace source {

class location;

/** A line location is a pair of a source origin and a line number. */
class line_location {

private:

    /** May be null. */
    std::shared_ptr<const location> m_parent;
    /** Non-null. */
    std::shared_ptr<const class origin> m_origin;
    /** Counted from 0. */
    std::size_t m_line;

public:

    line_location(
            std::shared_ptr<const location> &&parent, // may be null
            std::shared_ptr<const class origin> &&origin, // must never be null
            std::size_t line);

    line_location(const line_location &) = default;
    line_location(line_location &&) = default;
    line_location &operator=(const line_location &) = default;
    line_location &operator=(line_location &&) = default;
    ~line_location() = default;

    const location *parent() const noexcept { return m_parent.get(); }
    const class origin &origin() const noexcept { return *m_origin; }
    const std::size_t &line() const noexcept { return m_line; }

}; // class line_location

/**
 * Compares two line locations. The parents and origins are compared by their
 * addresses.
 */
bool operator==(const line_location &l, const line_location &r) noexcept;

inline bool operator!=(const line_location &l, const line_location &r)
        noexcept {
    return !(l == r);
}

/** A location points to a character position in a shell script source file. */
class location : public line_location {

private:

    /** Counted from 0. */
    std::size_t m_column;

public:

    location(const line_location &ll, std::size_t column) noexcept :
            line_location(ll), m_column(column) { }

    location(line_location &&ll, std::size_t column) noexcept :
            line_location(std::move(ll)), m_column(column) { }

    location(
            std::shared_ptr<const location> &&parent, // may be null
            std::shared_ptr<const class origin> &&origin, // must never be null
            std::size_t line,
            std::size_t column);

    location(const location &) = default;
    location(location &&) = default;
    location &operator=(const location &) = default;
    location &operator=(location &&) = default;
    ~location() = default;

    std::size_t column() const noexcept { return m_column; }

}; // class location

bool operator==(const location &l, const location &r) noexcept;

inline bool operator!=(const location &l, const location &r) noexcept {
    return !(l == r);
}

bool operator==(const line_location &, const location &) = delete;
bool operator!=(const line_location &, const location &) = delete;
bool operator==(const location &, const line_location &) = delete;
bool operator!=(const location &, const line_location &) = delete;

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_location_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
