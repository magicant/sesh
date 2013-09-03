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

#ifndef INCLUDED_language_source_Source_hh
#define INCLUDED_language_source_Source_hh

#include <memory>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/SourceLocation.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * Source is an abstract string that is parsed by the parser. An instance of
 * this class represents a string that is created by altering part of another
 * source string.
 */
class Source {

public:

    using String = common::String;
    using Char = String::value_type;
    using Size = String::size_type;
    using Difference = String::difference_type;
    using ConstReference = String::const_reference;
    constexpr static Char NEWLINE = L('\n');

    using Pointer = std::unique_ptr<const Source>;

private:

    /**
     * Original source string whose part is to be altered by this source
     * instance. The original may be null, in which case it is treated as an
     * empty string.
     */
    Pointer mOriginal;

    /**
     * Range of the original source string that is altered by this source
     * instance. (begin <= end <= length of the original)
     */
    Size mBegin, mEnd;

    /** String that alternates part of the original. */
    String mAlternate;

protected:

    const Pointer &original() const noexcept { return mOriginal; }
    Size begin() const noexcept { return mBegin; }
    Size end() const noexcept { return mEnd; }
    const String &alternate() const noexcept { return mAlternate; }

    Size positionAfterAlternate() const noexcept;
    Difference lengthDifference() const noexcept;

public:

    /**
     * @throws std::out_of_range begin > end || end > length of original
     * @throws std::overflow_error The resultant source is too long.
     */
    Source(Pointer &&original, Size begin, Size end, String &&alternate);

    Source(const Source &) = delete;
    Source(Source &&) = default;
    Source &operator=(const Source &) = delete;
    Source &operator=(Source &&) = default;
    virtual ~Source() = default;

    Size length() const noexcept;

    ConstReference at(Size) const;
    ConstReference operator[](Size) const;

private:

    /**
     * Computes the position of the first character of the line at the argument
     * position in the alternate.
     * @param position <= length of alternate
     * @return position in the alternate (<= length)
     */
    virtual Size lineBeginInAlternate(Size position) const noexcept;
    /**
     * Computes the character position just past the line at the argument
     * position in the alternate. If the line does not have a trailing newline
     * in the alternate, String::npos is returned.
     * @param position < length of alternate
     * @return position in the alternate (<= length) or String::npos
     */
    virtual Size lineEndInAlternate(Size position) const noexcept;

    /**
     * @param position < length of alternate
     * @return source location for the argument position in the alternate
     */
    virtual SourceLocation locationInAlternate(Size position) const = 0;

public:

    /**
     * Computes the position of the first character of the line at the argument
     * position in this source.
     * @param position <= length of this source
     * @return position in this source (<= length)
     */
    Size lineBegin(Size position) const noexcept;
    /**
     * Computes the character position just past the line at the argument
     * position in this source. If the line does not have a trailing newline,
     * the length of this source is returned.
     * @param position < length of this source
     * @return position in this source (<= length)
     */
    Size lineEnd(Size position) const noexcept;

    /**
     * @param position < length of this source
     * @return source location for the argument position
     */
    SourceLocation location(Size position) const;

};

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_Source_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
