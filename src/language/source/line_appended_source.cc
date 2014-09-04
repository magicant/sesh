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
#include "line_appended_source.hh"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

line_appended_source::line_appended_source(
        source_pointer &&original,
        size_type begin,
        size_type end,
        string_type &&alternate,
        LineLocation &&line_location) :
        source(std::move(original), begin, end, std::move(alternate)),
        m_line_location(std::move(line_location)) { }

line_appended_source line_appended_source::create(
        source_pointer &&original,
        string_type &&line,
        LineLocation &&location) {
    size_type newline_position = line.find(newline);
    if (newline_position != string_type::npos)
        if (newline_position != line.length() - 1)
            throw std::invalid_argument("newline at illegal position");

    size_type length = (original == nullptr) ? 0 : original->length();
    return line_appended_source(
            std::move(original),
            length,
            length,
            std::move(line),
            std::move(location));
}

auto line_appended_source::line_begin_in_alternate(size_type position)
        const noexcept -> size_type {
    if (position == alternate().length())
        if (position > 0 && alternate()[position - 1] == newline)
            return position;
    return 0;
}

auto line_appended_source::line_end_in_alternate(size_type)
        const noexcept -> size_type {
    size_type alt_length = alternate().length();
    if (alt_length == 0 || alternate()[alt_length - 1] != newline)
        return string_type::npos;
    return alt_length;
}

class location line_appended_source::location_in_alternate(size_type position)
        const {
    class location l(m_line_location, position);
    return l;
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
