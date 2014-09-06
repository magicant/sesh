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

#include "buildconfig.h"
#include "and_or_list.hh"

#include <utility>
#include "common/xchar.hh"
#include "language/syntax/Printer.hh"

namespace sesh {
namespace language {
namespace syntax {

and_or_list::and_or_list(pipeline &&first, synchronicity_type s) :
        m_first(std::move(first)), m_rest(), m_synchronicity(s) { }

namespace {

inline void print_separator(and_or_list::synchronicity_type s, Printer &p) {
    p.clearDelayedCharacters();
    switch (s) {
    case and_or_list::synchronicity_type::sequential:
        p.delayedCharacters() << L(';');
        break;
    case and_or_list::synchronicity_type::asynchronous:
        p << L('&');
        break;
    }
    p.delayedCharacters() << L(' ');
}

} // namespace

void and_or_list::print(Printer &p) const {
    p << first();
    for (const conditional_pipeline &cp : rest())
        p << cp;
    print_separator(synchronicity(), p);
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
