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
        Pointer &&original,
        Size begin,
        Size end,
        String &&alternate,
        LineLocation &&lineLocation) :
        Source(std::move(original), begin, end, std::move(alternate)),
        mLineLocation(std::move(lineLocation)) { }

LineAppendedSource LineAppendedSource::create(
        Pointer &&original, String &&line, LineLocation &&location) {
    Size newlinePosition = line.find(NEWLINE);
    if (newlinePosition != String::npos)
        if (newlinePosition != line.length() - 1)
            throw std::invalid_argument("newline at illegal position");

    Size length = (original == nullptr) ? 0 : original->length();
    return LineAppendedSource(
            std::move(original),
            length,
            length,
            std::move(line),
            std::move(location));
}

auto LineAppendedSource::lineBeginInAlternate(Size position)
        const noexcept -> Size {
    if (position == alternate().length())
        if (position > 0 && alternate()[position - 1] == NEWLINE)
            return position;
    return 0;
}

auto LineAppendedSource::lineEndInAlternate(Size) const noexcept -> Size {
    Size altLength = alternate().length();
    if (altLength == 0 || alternate()[altLength - 1] != NEWLINE)
        return String::npos;
    return altLength;
}

Location LineAppendedSource::locationInAlternate(Size position) const {
    return Location(mLineLocation, position);
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
