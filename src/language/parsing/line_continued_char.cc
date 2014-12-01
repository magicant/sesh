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
#include "line_continued_char.hh"

#include <functional>
#include <tuple>
#include "async/future.hh"
#include "common/constant_function.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/char.hh"
#include "language/parsing/joiner.hh"
#include "language/parsing/line_continuation.hh"
#include "language/parsing/mapper.hh"

namespace {

using sesh::async::future;
using sesh::common::constant;
using sesh::common::xchar;
using sesh::common::xchar_traits;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

future<result<xchar>> test_char_after_line_continuations(
        const std::function<char_predicate> &p, const state &s) {
    using namespace std::placeholders;
    return map_value(
            join(skip_line_continuations, std::bind(test_char, p, _1))(s),
            static_cast<xchar &&(*)(std::tuple<blackhole, xchar> &&)>(
                std::get<1>));
}

future<result<xchar>> parse_char_after_line_continuations(
        xchar c, const state &s) {
    using namespace std::placeholders;
    return test_char_after_line_continuations(
            std::bind(xchar_traits::eq, c, _1), s);
}

future<result<xchar>> accept_char_after_line_continuations(const state &s) {
    return test_char_after_line_continuations(constant(true), s);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
