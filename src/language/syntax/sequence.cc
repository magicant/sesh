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
#include "sequence.hh"

#include "language/syntax/and_or_list.hh"
#include "language/syntax/printer.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

inline void print_separator(printer &p) {
    switch (p.line_mode()) {
    case printer::line_mode_type::single_line:
        break;
    case printer::line_mode_type::multi_line:
        p.break_line();
        p.print_indent();
        break;
    }
}

} // namespace

void sequence::print(printer &p) const {
    bool is_first = true;
    for (const and_or_list_pointer &aol : and_or_lists()) {
        if (!is_first)
            print_separator(p);
        p << *aol;
        is_first = false;
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
