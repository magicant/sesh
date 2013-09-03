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

#ifndef INCLUDED_language_source_SourceLineLocationTestHelper_hh
#define INCLUDED_language_source_SourceLineLocationTestHelper_hh

#include <cstddef>
#include <memory>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/source/SourceLineLocation.hh"

namespace sesh {
namespace language {
namespace source {

/** For testing only. */
inline SourceLineLocation dummySourceLineLocation(
        const common::Char *name = L("dummy"),
        std::size_t line = 0) {
    return SourceLineLocation(std::make_shared<common::String>(name), line);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_SourceLineLocationTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
