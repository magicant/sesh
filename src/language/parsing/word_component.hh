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

#ifndef INCLUDED_language_parsing_word_component_hh
#define INCLUDED_language_parsing_word_component_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include "async/future.hh"
#include "language/parsing/char_predicate.hh"
#include "language/parsing/parser.hh"
#include "language/syntax/word_component.hh"

namespace sesh {
namespace language {
namespace parsing {

using word_component_pointer = std::shared_ptr<const syntax::word_component>;

/**
 * Parses a word component, possibly including line continuations.
 *
 * The argument char predicate is used to decide what characters can be
 * included in the word component if it is a raw_string.
 *
 * The parser fails with no error reports if there is no word component. The
 * parser fails with some error report if a syntax error is found.
 *
 * If successful, the resultant word component pointer is never null.
 */
auto parse_word_component(const std::function<char_predicate> &, const state &)
        -> async::future<result<word_component_pointer>>;

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_word_component_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
