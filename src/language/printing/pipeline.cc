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

#include "buildconfig.h"
#include "pipeline.hh"

#include <memory>
#include "common/xchar.hh"
#include "language/printing/command.hh"
#include "language/syntax/command.hh"

namespace {

using sesh::language::syntax::command;
using sesh::language::syntax::pipeline;

} // namespace

namespace sesh {
namespace language {
namespace printing {

void print(const pipeline &p, buffer &b) {
    switch (p.exit_status_mode) {
    case pipeline::exit_status_mode_type::straight:
        break;
    case pipeline::exit_status_mode_type::negated:
        b.append_main(L("!"));
        b.append_delayed_characters(L(' '));
        break;
    }

    for (const std::shared_ptr<const command> &c : p.commands) {
        if (&c != &p.commands.front()) {
            b.append_main(L('|'));
            b.append_delayed_characters(L(' '));
        }
        print(*c, b);
    }
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
