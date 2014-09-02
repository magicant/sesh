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

#ifndef INCLUDED_language_source_source_hh
#define INCLUDED_language_source_source_hh

#include "buildconfig.h"

#include <memory>
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/source/Location.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * Source is an abstract string that is parsed by the parser. An instance of
 * this class represents a string that is created by altering part of another
 * source string.
 */
class source {

public:

    using string_type = common::xstring;
    using value_type = string_type::value_type;
    using size_type = string_type::size_type;
    using difference_type = string_type::difference_type;
    using const_reference = string_type::const_reference;
    constexpr static value_type newline = L('\n');

    using source_pointer = std::unique_ptr<const source>;

private:

    source_pointer m_original;
    size_type m_begin, m_end;
    string_type m_alternate;

protected:

    /**
     * Original source string whose part is to be altered by this source
     * instance. The original may be null, in which case it is treated as an
     * empty string.
     */
    const source_pointer &original() const noexcept { return m_original; }
    /**
     * Beginning position of the range in the original to be replaced.
     * Inclusive. The position cannot exceed the length of the original.
     */
    size_type begin() const noexcept { return m_begin; }
    /**
     * Ending position of the range in the original to be replaced. Exclusive.
     * The position cannot exceed the length of the original.
     */
    size_type end() const noexcept { return m_end; }
    /** String that alternates part of the original. */
    const string_type &alternate() const noexcept { return m_alternate; }

    size_type position_after_alternate() const noexcept;
    difference_type length_difference() const noexcept;

public:

    /**
     * @throws std::out_of_range begin > end || end > length of original
     * @throws std::overflow_error The resultant source is too long.
     */
    source(
            source_pointer &&original,
            size_type begin,
            size_type end,
            string_type &&alternate);

    source(const source &) = delete;
    source(source &&) = default;
    source &operator=(const source &) = delete;
    source &operator=(source &&) = default;
    virtual ~source() = default;

    size_type length() const noexcept;

    const_reference at(size_type) const;
    const_reference operator[](size_type) const;

private:

    /**
     * Computes the position of the first character of the line at the argument
     * position in the alternate.
     * @param position <= length of alternate
     * @return position in the alternate (<= length)
     */
    virtual size_type line_begin_in_alternate(size_type position) const
            noexcept;
    /**
     * Computes the character position just past the line at the argument
     * position in the alternate. If the line does not have a trailing newline
     * in the alternate, string_type::npos is returned.
     * @param position < length of alternate
     * @return position in the alternate (<= length) or string_type::npos
     */
    virtual size_type line_end_in_alternate(size_type position) const noexcept;

    /**
     * @param position < length of alternate
     * @return source location for the argument position in the alternate
     */
    virtual Location location_in_alternate(size_type position) const = 0;

public:

    /**
     * Computes the position of the first character of the line at the argument
     * position in this source.
     * @param position <= length of this source
     * @return position in this source (<= length)
     */
    size_type line_begin(size_type position) const noexcept;
    /**
     * Computes the character position just past the line at the argument
     * position in this source. If the line does not have a trailing newline,
     * the length of this source is returned.
     * @param position < length of this source
     * @return position in this source (<= length)
     */
    size_type line_end(size_type position) const noexcept;

    /**
     * @param position < length of this source
     * @return source location for the argument position
     */
    Location location(size_type position) const;

};

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_source_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
