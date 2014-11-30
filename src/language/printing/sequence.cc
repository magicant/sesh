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
#include "sequence.hh"

#include "language/printing/and_or_list.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/sequence.hh"

namespace {

using sesh::language::syntax::and_or_list;
using sesh::language::syntax::sequence;

} // namespace

namespace sesh {
namespace language {
namespace printing {

void print(const sequence &s, buffer &b) {
    for (const and_or_list &l : s.and_or_lists) {
        if (&l != &s.and_or_lists.front()) {
            b.break_line();
            b.indent();
        }
        print(l, b);
    }
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
