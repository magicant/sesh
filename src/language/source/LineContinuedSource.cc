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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#include "common.hh"
#include "LineContinuedSource.hh"

#include <exception>
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

LineContinuedSource::LineContinuedSource(
        Pointer &&original, Size position) :
        Source(std::move(original), position, position + 2, String()) {
    const Source &o = *this->original();
    if (o[position] != L'\\' || o[position + 1] != NEWLINE)
        throw std::invalid_argument("no line continuation");
}

SourceLocation LineContinuedSource::locationInAlternate(Size) const {
    // The length of the alternate is always zero in this class, so this
    // function is never called.
    std::terminate();
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
