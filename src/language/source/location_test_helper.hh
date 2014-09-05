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

#ifndef INCLUDED_language_source_location_test_helper_hh
#define INCLUDED_language_source_location_test_helper_hh

#include "buildconfig.h"

#include <cstddef>
#include <memory>
#include "language/source/location.hh"
#include "language/source/OriginTestHelper.hh"

namespace sesh {
namespace language {
namespace source {

/** For testing only. */
inline line_location dummy_line_location(std::size_t line = 1) {
    return line_location(nullptr, dummyOrigin(), line);
}

/** For testing only. */
inline location dummy_location(std::size_t line = 1, std::size_t column = 1) {
    return location(dummy_line_location(line), column);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_location_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
