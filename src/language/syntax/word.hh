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

#ifndef INCLUDED_language_syntax_word_hh
#define INCLUDED_language_syntax_word_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/word_component.hh"

namespace sesh {
namespace language {
namespace syntax {

/** A word is a token that is made up of zero or more word components. */
class word {

public:

    /** The type of pointers to a word component. Pointers must not be null. */
    using component_pointer = std::shared_ptr<const word_component>;

    std::vector<component_pointer> components;

}; // class word

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_word_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
