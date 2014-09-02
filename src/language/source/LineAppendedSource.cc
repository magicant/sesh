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
#include "LineAppendedSource.hh"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

LineAppendedSource::LineAppendedSource(
        source_pointer &&original,
        size_type begin,
        size_type end,
        string_type &&alternate,
        LineLocation &&lineLocation) :
        source(std::move(original), begin, end, std::move(alternate)),
        mLineLocation(std::move(lineLocation)) { }

LineAppendedSource LineAppendedSource::create(
        source_pointer &&original,
        string_type &&line,
        LineLocation &&location) {
    size_type newlinePosition = line.find(newline);
    if (newlinePosition != string_type::npos)
        if (newlinePosition != line.length() - 1)
            throw std::invalid_argument("newline at illegal position");

    size_type length = (original == nullptr) ? 0 : original->length();
    return LineAppendedSource(
            std::move(original),
            length,
            length,
            std::move(line),
            std::move(location));
}

auto LineAppendedSource::line_begin_in_alternate(size_type position)
        const noexcept -> size_type {
    if (position == alternate().length())
        if (position > 0 && alternate()[position - 1] == newline)
            return position;
    return 0;
}

auto LineAppendedSource::line_end_in_alternate(size_type)
        const noexcept -> size_type {
    size_type altLength = alternate().length();
    if (altLength == 0 || alternate()[altLength - 1] != newline)
        return string_type::npos;
    return altLength;
}

Location LineAppendedSource::location_in_alternate(size_type position) const {
    return Location(mLineLocation, position);
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
