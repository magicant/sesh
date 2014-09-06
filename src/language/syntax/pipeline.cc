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
#include "pipeline.hh"

#include <utility>
#include <vector>
#include "common/xchar.hh"
#include "language/syntax/printer.hh"

namespace sesh {
namespace language {
namespace syntax {

pipeline::pipeline(exit_status_mode_type e) :
        m_commands(), m_exit_status_mode(e) { }

void pipeline::print(printer &p) const {
    switch (exit_status_mode()) {
    case exit_status_mode_type::straight:
        break;
    case exit_status_mode_type::negated:
        p << L("! ");
        break;
    }

    bool is_first = true;
    for (const command_pointer &c : commands()) {
        if (!is_first)
            p << L("| ");
        p << *c;
        is_first = false;
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
