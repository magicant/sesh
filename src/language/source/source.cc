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
    size_type original_length =
            (m_original == nullptr) ? 0 : m_original->length();
    difference_type ld = length_difference();
    size_type this_length = original_length + ld;

    if (m_begin > m_end || m_end > original_length)
        throw std::out_of_range("invalid alteration range");
    if (ld > 0 && this_length < original_length)
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

    size_type position_from_begin = position - begin();
    if (position_from_begin < alternate().length())
        return alternate().at(position_from_begin);

    size_type position_from_end =
            position_from_begin - alternate().length() + end();
    if (position_from_end < end())
        position_from_end = string_type::npos; // recover overflow
    return original()->at(position_from_end);
}

auto source::operator[](size_type position) const -> const_reference {
    if (original() == nullptr)
        return alternate()[position];

    if (position < begin())
        return (*original())[position];

    size_type position_from_begin = position - begin();
    if (position_from_begin < alternate().length())
        return alternate()[position_from_begin];

    return (*original())[position_from_begin - alternate().length() + end()];
}

auto source::line_begin_in_alternate(size_type position) const noexcept
        -> size_type {
    if (position == 0)
        return 0;

    size_type newline_position = alternate().rfind(newline, position - 1);
    if (newline_position == string_type::npos)
        return 0;
    return newline_position + 1;
}

auto source::line_end_in_alternate(size_type position) const noexcept
        -> size_type {
    size_type newline_position = alternate().find(newline, position);
    if (newline_position == string_type::npos)
        return string_type::npos;
    return newline_position + 1;
}

auto source::line_begin(size_type position) const noexcept -> size_type {
    if (original() != nullptr && position > position_after_alternate()) {
        difference_type ld = length_difference();
        size_type line_begin_after_alternate =
                original()->line_begin(position - ld);
        if (line_begin_after_alternate > end())
            return line_begin_after_alternate + ld;
        position = position_after_alternate();
    }

    if (position > begin()) {
        size_type position_in_alternate = position - begin();
        size_type alt_line_begin =
                line_begin_in_alternate(position_in_alternate);
        if (alt_line_begin > 0)
            return alt_line_begin + begin();
        position = begin();
    }

    if (original() == nullptr)
        return 0;
    return original()->line_begin(position);
}

auto source::line_end(size_type position) const noexcept -> size_type {
    if (position < begin()) {
        size_type line_end_before_begin = original()->line_end(position);
        if (line_end_before_begin < begin())
            return line_end_before_begin;
        if (line_end_before_begin == begin())
            if ((*original())[line_end_before_begin - 1] == newline)
                return line_end_before_begin;
        position = begin();
    }

    if (position < position_after_alternate()) {
        size_type position_in_alternate = position - begin();
        size_type alt_line_end = line_end_in_alternate(position_in_alternate);
        if (alt_line_end != string_type::npos)
            return alt_line_end + begin();
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
