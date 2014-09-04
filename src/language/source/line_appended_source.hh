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

#ifndef INCLUDED_language_source_line_appended_source_hh
#define INCLUDED_language_source_line_appended_source_hh

#include "buildconfig.h"

#include "language/source/Location.hh"
#include "language/source/source.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * Line-appended source represents a source string that is created by appending
 * a line of string to another source instance. The appended line is supposed
 * to be taken from an actual source code. A line-appended source instance
 * contains information about the source code name and line number.
 */
class line_appended_source : public source {

private:

    LineLocation m_line_location;

    line_appended_source(
            source_pointer &&,
            size_type,
            size_type,
            string_type &&,
            LineLocation &&);

public:

    /**
     * Creates a line-appended source instance with argument validity check. If
     * the appended line contains a newline character, it must be at the end of
     * the line.
     */
    static line_appended_source create(
            source_pointer &&original, string_type &&line, LineLocation &&);

    line_appended_source(const line_appended_source &) = delete;
    line_appended_source(line_appended_source &&) = default;
    line_appended_source &operator=(const line_appended_source &) = delete;
    line_appended_source &operator=(line_appended_source &&) = default;
    ~line_appended_source() override = default;

private:

    size_type line_begin_in_alternate(size_type) const noexcept override;
    size_type line_end_in_alternate(size_type) const noexcept override;
    Location location_in_alternate(size_type) const override;

};

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_line_appended_source_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
