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
#include "line_continuation.hh"

#include <functional>
#include "async/future.hh"
#include "common/constant_function.hh"
#include "common/empty.hh"
#include "common/xchar.hh"
#include "language/parsing/blackhole.hh"
#include "language/parsing/char.hh"
#include "language/parsing/joiner.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/repeat.hh"

namespace {

using sesh::async::future;
using sesh::common::empty;
using sesh::common::xchar;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

auto skip_line_continuation(const state &s)
        -> future<result<std::tuple<xchar, xchar>>> {
    using namespace std::placeholders;
    static const auto p = join(
            std::bind(parse_char, L('\\'), _1),
            std::bind(parse_char, L('\n'), _1));
    return p(s);
}

future<result<blackhole>> skip_line_continuations(const state &s) {
    return repeat(skip_line_continuation, s, blackhole());
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
