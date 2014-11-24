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
#include "whitespace.hh"

#include <functional>
#include <utility>
#include "async/future.hh"
#include "common/constant_function.hh"
#include "common/empty.hh"
#include "language/parsing/blackhole.hh"
#include "language/parsing/char_predicate.hh"
#include "language/parsing/comment.hh"
#include "language/parsing/joiner.hh"
#include "language/parsing/line_continuation.hh"
#include "language/parsing/line_continued_char.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/option.hh"
#include "language/parsing/repeat.hh"

namespace {

using sesh::async::future;
using sesh::common::empty;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

namespace {

future<result<blackhole>> skip_blanks(const state &s) {
    using std::placeholders::_1;
    return repeat(
            std::bind(test_char_after_line_continuations, is_blank, _1),
            s,
            blackhole());
}

} // namespace

future<result<empty>> skip_whitespaces(const state &s) {
    static const auto p = join(
            skip_blanks, skip_line_continuations, option(skip_comment));
    return map_value(p(s), constant(empty()));
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
