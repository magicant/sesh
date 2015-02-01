/* Copyright (C) 2015 WATANABE Yuki
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

#ifndef INCLUDED_language_executing_word_char_hh
#define INCLUDED_language_executing_word_char_hh

#include "buildconfig.h"

#include "common/xchar.hh"

namespace sesh {
namespace language {
namespace executing {

/** Character that results from word (character) expansion. */
class word_char {

public:

    /** The character value. */
    common::xchar character;

    /**
     * Whether this character originates from a literal or an expansion. This
     * affects whether this character is subject to word splitting.
     */
    bool is_literal : 1;

    /**
     * Whether this character originates from a quotation or escape. This
     * affects whether this character is subject to globbing.
     */
    bool is_quoted : 1;

    /** Implicit conversion to xchar. */
    constexpr operator common::xchar() const noexcept { return character; }

    /** Conversion to Boolean is disabled. */
    constexpr operator bool() const = delete;

}; // class word_char

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_word_char_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
