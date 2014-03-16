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

#ifndef INCLUDED_language_source_LineLocation_hh
#define INCLUDED_language_source_LineLocation_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include "common/String.hh"

namespace sesh {
namespace language {
namespace source {

/** A line location is a pair of a source name and a line number. */
class LineLocation {

private:

    /** Not necessarily a filename. */
    std::shared_ptr<const common::String> mName;
    /** Counted from 0. */
    std::size_t mLine;

public:

    LineLocation(const std::shared_ptr<const common::String> &, std::size_t);
    LineLocation(std::shared_ptr<const common::String> &&, std::size_t);

    LineLocation(const LineLocation &) = default;
    LineLocation(LineLocation &&) = default;
    LineLocation &operator=(const LineLocation &) = default;
    LineLocation &operator=(LineLocation &&) = default;
    ~LineLocation() = default;

    const common::String &name() const noexcept { return *mName; }
    const std::size_t &line() const noexcept { return mLine; }

};

bool operator==(const LineLocation &l, const LineLocation &r);

inline bool operator!=(const LineLocation &l, const LineLocation &r) {
    return !(l == r);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_LineLocation_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
