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

#ifndef INCLUDED_language_source_LineContinuedSource_hh
#define INCLUDED_language_source_LineContinuedSource_hh

#include "buildconfig.h"

#include "language/source/source.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * Line-continued source is a source wrapper that removes a line continuation
 * from the original source.
 */
class LineContinuedSource : public Source {

public:

    /**
     * Creates a line-continued source instance. The original source must have
     * a line continuation at the argument position.
     * @throws std::invalid_argument no line continuation at the position.
     * @throws std::out_of_range too large position.
     */
    LineContinuedSource(Pointer &&original, Size position);

    LineContinuedSource(const LineContinuedSource &) = delete;
    LineContinuedSource(LineContinuedSource &&) = default;
    LineContinuedSource &operator=(const LineContinuedSource &) = delete;
    LineContinuedSource &operator=(LineContinuedSource &&) = default;
    ~LineContinuedSource() override = default;

private:

    Location locationInAlternate(Size) const override;

};

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_LineContinuedSource_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
