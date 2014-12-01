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
#include "word.hh"

#include <functional>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/repeat.hh"
#include "language/parsing/word_component.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::language::syntax::word;

word to_word(std::vector<word_component_pointer> &&wcps) {
    return {std::move(wcps)};
}

}

future<result<word>> parse_word(
        const std::function<char_predicate> &p, const state &s) {
    using std::placeholders::_1;
    auto r = repeat(std::bind(parse_word_component, p, _1), s);
    return map_value(std::move(r), to_word);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
