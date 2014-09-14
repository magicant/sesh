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
#include "simple_command.hh"

#include <utility>
#include "common/xchar.hh"
#include "language/syntax/printer.hh"

namespace sesh {
namespace language {
namespace syntax {

simple_command::simple_command() : command(), m_words(), m_assignments() { }

void simple_command::print(printer &p) const {
    for (const assignment_pointer &a : assignments()) {
        p << *a;
        p.delayed_characters() << L(' ');
    }
    for (const word_pointer &w : words()) {
        p << *w;
        p.delayed_characters() << L(' ');
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */