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

#ifndef INCLUDED_language_syntax_raw_string_hh
#define INCLUDED_language_syntax_raw_string_hh

#include "buildconfig.h"

#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A raw string is a word component composed of a non-empty string.
 *
 * Despite that definition, an instance of this class may have an empty string.
 * Users of this class must ensure that the string is non-empty.
 */
class raw_string {

public:

    common::xstring value;

}; // class raw_string

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_raw_string_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
