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

#ifndef INCLUDED_language_source_fragment_test_helper_hh
#define INCLUDED_language_source_fragment_test_helper_hh

#include "buildconfig.h"

#include <ostream>
#include "language/source/fragment.hh"

namespace sesh {
namespace language {
namespace source {

template<typename Char, typename Traits>
std::basic_ostream<Char, Traits> &operator<<(
        std::basic_ostream<Char, Traits> &os,
        const fragment_position &fp) {
    return os << '{' << static_cast<const void *>(fp.head.get()) << ", " <<
            fp.index << '}';
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_fragment_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
