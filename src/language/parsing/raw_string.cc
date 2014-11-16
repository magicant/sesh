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
#include "raw_string.hh"

#include <functional>
#include "async/future.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/char.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/repeat.hh"
#include "language/syntax/raw_string.hh"

namespace {

using sesh::async::future;
using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::raw_string;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

auto parse_raw_string(const std::function<bool(xchar)> &p, const state &s)
        -> future<result<raw_string>> {
    return map_value(
            one_or_more(
                std::bind(test_char, p, std::placeholders::_1),
                s,
                xstring{}),
            [](xstring &&s) { return raw_string{std::move(s)}; });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
