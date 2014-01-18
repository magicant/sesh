/* Copyright (C) 2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_EnvironmentHelper_hh
#define INCLUDED_language_parser_EnvironmentHelper_hh

#include "buildconfig.h"

#include "common/String.hh"
#include "language/parser/Environment.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Returns the character at the given position in the given environment. The
 * position must be less than or equal to the length of the source buffer in
 * the environment. If the position is equal to the length,
 *  - EOF is returned if {@link Environment#isEof} returns true.
 *  - IncompleteParse is thrown otherwise.
 */
common::CharTraits::int_type charIntAt(const Environment &, Environment::Size);

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_EnvironmentHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
