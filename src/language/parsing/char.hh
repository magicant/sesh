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

#ifndef INCLUDED_language_parsing_char_hh
#define INCLUDED_language_parsing_char_hh

#include "buildconfig.h"

#include <functional>
#include "async/future.hh"
#include "common/xchar.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * Parses one character from the stream in the argument state.
 *
 * The character is tested by the argument predicate. Parsing succeeds if and
 * only if the predicate returns true.
 *
 * This parser never returns any report even on failure.
 */
auto test_char(const std::function<bool(common::xchar)> &, const state &)
        -> async::future<result<common::xchar>>;

/**
 * Expects the next character to be the argument character.
 *
 * This parser never returns any report even on failure.
 */
auto parse_char(common::xchar, const state &)
        -> async::future<result<common::xchar>>;

/**
 * Accepts the next character.
 *
 * This parser function always succeeds unless the end of input is reached.
 *
 * This parser never returns any report even on failure.
 */
auto accept_char(const state &)
        -> async::future<result<common::xchar>>;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_char_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
