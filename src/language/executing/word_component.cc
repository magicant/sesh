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
#include "word_component.hh"

#include <memory>
#include <utility>
#include "async/future.hh"
#include "common/either.hh"
#include "common/visitor.hh"
#include "environment/world.hh"
#include "language/executing/raw_string.hh"
#include "language/syntax/word_component.hh"

namespace sesh {
namespace language {
namespace executing {

namespace {

using sesh::async::future;
using sesh::common::maybe;
using sesh::environment::world;
using sesh::language::syntax::word_component;

class expander {

public:

    const std::shared_ptr<world> &w;
    bool is_quoted;
    const std::shared_ptr<const word_component> &wc;
    maybe<future<expansion_result>> &result;

    template<typename T>
    void operator()(const T &v) const {
        result = expand(w, is_quoted, std::shared_ptr<const T>(wc, &v));
    }

}; // class expander

} // namespace

future<expansion_result> expand(
        const std::shared_ptr<world> &w,
        bool is_quoted,
        const std::shared_ptr<const word_component> &wc) {
    maybe<future<expansion_result>> result;
    visit(*wc, expander{w, is_quoted, wc, result});
    return std::move(*result);
}

} // namespace executing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
