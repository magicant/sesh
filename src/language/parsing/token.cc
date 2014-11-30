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
#include "token.hh"

#include <utility>
#include "async/future.hh"
#include "common/empty.hh"
#include "common/type_tag.hh"
#include "common/type_tag_set.hh"
#include "language/parsing/char_predicate.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/word.hh"
#include "language/syntax/word.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::async::make_future;
using sesh::common::empty;
using sesh::common::type_tag;
using sesh::common::type_tag_set;
using sesh::language::syntax::word;

result<token> token_from_word(result<word> &&wr) {
    result<token> tr(empty(), std::move(wr.reports));
    if (wr.product && !wr.product->value.components.empty())
        tr.product.try_emplace(product<token>{
                std::move(wr.product->value), std::move(wr.product->state)});
    return tr;
}

} // namespace

future<result<token>> parse_token(token_type_set types, const state &s) {
    if (!types[type_tag<word>()])
        return make_future<result<token>>();
    return parse_word(is_token_char, s).map(token_from_word);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
