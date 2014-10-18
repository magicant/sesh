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

#ifndef INCLUDED_language_syntax_word_component_hh
#define INCLUDED_language_syntax_word_component_hh

#include "buildconfig.h"

#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace syntax {

class word_component {

public:

    virtual ~word_component() = default;

    virtual bool is_raw_string() const noexcept { return false; }

    /**
     * If this word component always evaluates to the same string in any shell
     * environment state, appends the string to the argument and returns true.
     * Otherwise, returns false without any side effects.
     */
    virtual bool append_constant_value(common::xstring &) const = 0;

}; // class word_component

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_word_component_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
