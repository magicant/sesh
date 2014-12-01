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
#include "simple_command.hh"

#include "language/printing/buffer.hh"
#include "language/printing/word.hh"
#include "language/syntax/simple_command.hh"

namespace {

using sesh::language::printing::buffer;
using sesh::language::syntax::simple_command;

} // namespace

namespace sesh {
namespace language {
namespace printing {

void print(const simple_command &c, buffer &b) {
    // TODO assignments
    for (auto w : c.words)
        print(w, b);
    // TODO redirections
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
