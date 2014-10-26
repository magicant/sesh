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
#include "and_or_list.hh"

#include "language/syntax/and_or_list.hh"
#include "language/printing/conditional_pipeline.hh"
#include "language/printing/pipeline.hh"

namespace {

using sesh::language::syntax::and_or_list;
using sesh::language::syntax::conditional_pipeline;

} // namespace

namespace sesh {
namespace language {
namespace printing {

void print(const and_or_list &l, buffer &b) {
    print(l.first, b);

    for (const conditional_pipeline &p : l.rest)
        print(p, b);

    b.clear_delayed_characters();
    switch (l.synchronicity) {
    case and_or_list::synchronicity_type::sequential:
        b.append_delayed_characters(L("; "));
        break;
    case and_or_list::synchronicity_type::asynchronous:
        b.append_main(L('&'));
        b.append_delayed_characters(L(' '));
        break;
    }
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
