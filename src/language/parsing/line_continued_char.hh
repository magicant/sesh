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

#ifndef INCLUDED_language_parsing_line_continued_char_hh
#define INCLUDED_language_parsing_line_continued_char_hh

#include "buildconfig.h"

#include "common/xchar.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * Skips line continuations and parses one following character.
 *
 * The character is tested by the argument predicate. Parsing succeeds if and
 * only if the predicate returns true.
 *
 * This parser never returns any report even on failure.
 */
auto test_char_after_line_continuations(
        const std::function<bool(common::xchar)> &, const state &)
        -> async::future<result<common::xchar>>;

/**
 * After skipping line continuations, expects the next character to be the
 * argument character.
 *
 * This parser never returns any report even on failure.
 */
auto parse_char_after_line_continuations(common::xchar, const state &)
        -> async::future<result<common::xchar>>;

/**
 * Skips line continuations and accepts the next character.
 *
 * This parser function always succeeds unless the end of input is reached.
 *
 * This parser never returns any report even on failure.
 */
auto accept_char_after_line_continuations(const state &)
        -> async::future<result<common::xchar>>;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_line_continued_char_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
