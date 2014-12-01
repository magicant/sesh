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
#include "word_component.hh"

#include <functional>
#include <memory>
#include <utility>
#include "async/future.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/raw_string.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word_component.hh"

namespace {

using sesh::async::future;
using sesh::common::contains;
using sesh::common::make_shared_visitable;
using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word_component;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

namespace {

const xstring special_chars = L("\"$'\\`");

template<typename T>
word_component_pointer to_word_component_pointer(T &&t) {
    return make_shared_visitable<word_component>(std::move(t));
}

}

future<result<word_component_pointer>> parse_word_component(
        const std::function<char_predicate> &p, const state &s) {
    // TODO support word component types other than raw string

    std::function<char_predicate> p2 = [p](xchar x, const context &c) {
        return !contains(special_chars, x) && p(x, c);
    };

    return map_value(
            parse_raw_string(p2, s), &to_word_component_pointer<raw_string>);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
