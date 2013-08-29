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

#ifndef INCLUDED_parser_LineAppendedSource_hh
#define INCLUDED_parser_LineAppendedSource_hh

#include "parser/Source.hh"
#include "parser/SourceLineLocation.hh"

namespace sesh {
namespace parser {

/**
 * Line-appended source represents a source string that is created by appending
 * a line of string to another source instance. The appended line is supposed
 * to be taken from an actual source code. A line-appended source instance
 * contains information about the source code name and line number.
 */
class LineAppendedSource : public Source {

private:

    SourceLineLocation mLineLocation;

    LineAppendedSource(
            Pointer &&, Size, Size, String &&, SourceLineLocation &&);

public:

    /**
     * Creates a line-appended source instance with argument validity check. If
     * the appended line contains a newline character, it must be at the end of
     * the line.
     */
    static LineAppendedSource create(
            Pointer &&original, String &&line, SourceLineLocation &&);

    LineAppendedSource(const LineAppendedSource &) = delete;
    LineAppendedSource(LineAppendedSource &&) = default;
    LineAppendedSource &operator=(const LineAppendedSource &) = delete;
    LineAppendedSource &operator=(LineAppendedSource &&) = default;
    ~LineAppendedSource() override = default;

private:

    Size lineBeginInAlternate(Size) const noexcept override;
    Size lineEndInAlternate(Size) const noexcept override;
    SourceLocation locationInAlternate(Size) const override;

};

} // namespace parser
} // namespace sesh

#endif // #ifndef INCLUDED_parser_LineAppendedSource_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
