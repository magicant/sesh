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

#ifndef INCLUDED_parser_SourceLocationTestHelper_hh
#define INCLUDED_parser_SourceLocationTestHelper_hh

#include <cstddef>
#include <memory>
#include <string>
#include "parser/SourceLocation.hh"

namespace sesh {
namespace parser {

/** For testing only. */
inline SourceLocation dummySourceLocation(
        const wchar_t *name = L"dummy",
        std::size_t line = 1,
        std::size_t column = 1) {
    return SourceLocation(
            std::make_shared<std::wstring>(name),
            line,
            column);
}

} // namespace parser
} // namespace sesh

#endif // #ifndef INCLUDED_parser_SourceLocationTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
