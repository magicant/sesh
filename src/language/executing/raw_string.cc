/* Copyright (C) 2015 WATANABE Yuki
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

#include <utility>
#include "async/future.hh"
#include "environment/world.hh"
#include "language/executing/string.hh"
#include "language/syntax/raw_string.hh"

namespace sesh {
namespace language {
namespace executing {

namespace {

using sesh::async::future;
using sesh::async::make_future_of;
using sesh::environment::world;
using sesh::language::syntax::raw_string;

} // namespace

future<expansion_result> expand(
        const std::shared_ptr<world> &,
        bool is_quoted,
        const std::shared_ptr<const raw_string> &rs) {
    expansion_result result;
    result.words.try_emplace();
    result.words->emplace_back();
    append(result.words->back().characters, rs->value, true, is_quoted);
    return make_future_of(std::move(result));
}

} // namespace executing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
