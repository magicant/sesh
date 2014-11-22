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
#include "comment.hh"

#include <functional>
#include <tuple>
#include "async/future.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/char.hh"
#include "language/parsing/joiner.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/repeat.hh"

namespace {

using sesh::async::future;
using sesh::common::xchar;
using sesh::common::xchar_traits;
using sesh::common::xstring;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

namespace {

const auto parse_hash_and_non_newlines = join(
        std::bind(parse_char, L('#'), std::placeholders::_1),
        [](const state &s) {
            return repeat(
                std::bind(
                    test_char,
                    [](xchar c, const context &) { return c != L('\n'); },
                    std::placeholders::_1),
                s,
                xstring{});
        });

} // namespace

future<result<xstring>> skip_comment(const state &s) {
    return map_value(
            parse_hash_and_non_newlines(s),
            static_cast<xstring &&(*)(std::tuple<xchar, xstring> &&t)>(
                std::get<1>));
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
