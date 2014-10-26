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
#include "conditional_pipeline.hh"

#include "common/xchar.hh"
#include "language/printing/pipeline.hh"
#include "language/syntax/conditional_pipeline.hh"

namespace {

using sesh::language::syntax::conditional_pipeline;

} // namespace

namespace sesh {
namespace language {
namespace printing {

void print(const conditional_pipeline &p, buffer &b) {
    switch (p.condition) {
    case conditional_pipeline::condition_type::and_then:
        b.append_main(L("&&"));
        break;
    case conditional_pipeline::condition_type::or_else:
        b.append_main(L("||"));
        break;
    }

    switch (b.line_mode()) {
    case buffer::line_mode_type::single_line:
        b.append_delayed_characters(L(' '));
        break;
    case buffer::line_mode_type::multi_line:
        b.break_line();
        b.indent();
        break;
    }

    print(p.pipeline, b);
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
