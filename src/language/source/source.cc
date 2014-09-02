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
#include "source.hh"

#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

auto source::position_after_alternate() const noexcept -> size_type {
    return begin() + alternate().length();
}

auto source::length_difference() const noexcept -> difference_type {
    return static_cast<difference_type>(alternate().length()) -
            static_cast<difference_type>(end() - begin());
}

source::source(
        source_pointer &&original,
        size_type begin,
        size_type end,
        string_type &&alternate) :
        m_original(std::move(original)),
        m_begin(begin),
        m_end(end),
        m_alternate(std::move(alternate)) {
    size_type originalLength =
            (m_original == nullptr) ? 0 : m_original->length();
    difference_type ld = length_difference();
    size_type thisLength = originalLength + ld;

    if (m_begin > m_end || m_end > originalLength)
        throw std::out_of_range("invalid alteration range");
    if (ld > 0 && thisLength < originalLength)
        throw std::overflow_error("too long source");
}

auto source::length() const noexcept -> size_type {
    if (original() == nullptr)
        return alternate().length();

    return original()->length() + length_difference();
}

auto source::at(size_type position) const -> const_reference {
    if (original() == nullptr)
        return alternate().at(position);

    if (position < begin())
        return original()->at(position);

    size_type positionFromBegin = position - begin();
    if (positionFromBegin < alternate().length())
        return alternate().at(positionFromBegin);

    size_type positionFromEnd =
            positionFromBegin - alternate().length() + end();
    if (positionFromEnd < end())
        positionFromEnd = string_type::npos; // recover overflow
    return original()->at(positionFromEnd);
}

auto source::operator[](size_type position) const -> const_reference {
    if (original() == nullptr)
        return alternate()[position];

    if (position < begin())
        return (*original())[position];

    size_type positionFromBegin = position - begin();
    if (positionFromBegin < alternate().length())
        return alternate()[positionFromBegin];

    return (*original())[positionFromBegin - alternate().length() + end()];
}

auto source::line_begin_in_alternate(size_type position) const noexcept
        -> size_type {
    if (position == 0)
        return 0;

    size_type newlinePosition = alternate().rfind(newline, position - 1);
    if (newlinePosition == string_type::npos)
        return 0;
    return newlinePosition + 1;
}

auto source::line_end_in_alternate(size_type position) const noexcept
        -> size_type {
    size_type newlinePosition = alternate().find(newline, position);
    if (newlinePosition == string_type::npos)
        return string_type::npos;
    return newlinePosition + 1;
}

auto source::line_begin(size_type position) const noexcept -> size_type {
    if (original() != nullptr && position > position_after_alternate()) {
        difference_type ld = length_difference();
        size_type lineBeginAfterAlternate =
                original()->line_begin(position - ld);
        if (lineBeginAfterAlternate > end())
            return lineBeginAfterAlternate + ld;
        position = position_after_alternate();
    }

    if (position > begin()) {
        size_type positionInAlternate = position - begin();
        size_type altLineBegin = line_begin_in_alternate(positionInAlternate);
        if (altLineBegin > 0)
            return altLineBegin + begin();
        position = begin();
    }

    if (original() == nullptr)
        return 0;
    return original()->line_begin(position);
}

auto source::line_end(size_type position) const noexcept -> size_type {
    if (position < begin()) {
        size_type lineEndBeforeBegin = original()->line_end(position);
        if (lineEndBeforeBegin < begin())
            return lineEndBeforeBegin;
        if (lineEndBeforeBegin == begin())
            if ((*original())[lineEndBeforeBegin - 1] == newline)
                return lineEndBeforeBegin;
        position = begin();
    }

    if (position < position_after_alternate()) {
        size_type positionInAlternate = position - begin();
        size_type altLineEnd = line_end_in_alternate(positionInAlternate);
        if (altLineEnd != string_type::npos)
            return altLineEnd + begin();
        position = position_after_alternate();
    }

    if (original() == nullptr)
        return position;

    difference_type ld = length_difference();
    return original()->line_end(position - ld) + ld;
}

Location source::location(size_type position) const {
    if (position < begin())
        return original()->location(position);
    if (position < position_after_alternate())
        return location_in_alternate(position - begin());
    return original()->location(position - length_difference());
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
