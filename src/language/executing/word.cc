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
#include "word.hh"

#include <algorithm>
#include <memory>
#include "async/continuation.hh"
#include "async/future.hh"
#include "common/container_helper.hh"
#include "common/either.hh"
#include "common/shared_function.hh"
#include "environment/world.hh"
#include "language/executing/word_component.hh"
#include "language/syntax/word.hh"

namespace sesh {
namespace language {
namespace executing {

namespace {

using sesh::async::continuation;
using sesh::async::future;
using sesh::async::make_future_of;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::move;
using sesh::common::shared_function;
using sesh::common::trial;
using sesh::environment::world;
using sesh::language::syntax::word;

using components = std::vector<word::component_pointer>;

void join(std::vector<expansion> &to, std::vector<expansion> &&from) {
    if (from.empty())
        return;
    if (to.empty()) {
        to = std::move(from);
        return;
    }

    move(from.back().characters, to.front().characters);
    // TODO: std::move(++from.begin(), from.end(), std::back_inserter(to));
}

class four_expansion_state :
        public std::enable_shared_from_this<four_expansion_state> {

private:

    std::shared_ptr<world> m_world;
    bool m_is_quoted;
    std::shared_ptr<const components> m_components;
    components::const_iterator m_next_component;
    expansion_result m_result;

public:

    four_expansion_state(
            const std::shared_ptr<world> &w,
            bool is_quoted,
            std::shared_ptr<const components> &&cs) :
            m_world(w),
            m_is_quoted(is_quoted),
            m_components(std::move(cs)),
            m_next_component(m_components->begin()),
            m_result() {
        m_result.words.try_emplace();
    }

    future<expansion_result> proceed() {
        if (m_next_component == m_components->end())
            return make_future_of(std::move(m_result));

        auto f = expand(m_world, m_is_quoted, *m_next_component);
        ++m_next_component;
        return std::move(f)
                .map(shared_function<four_expansion_state>(shared_from_this()))
                .unwrap();
    }

    /** Accepts result of word component expansion. */
    future<expansion_result> operator()(expansion_result &&er) {
        move(er.reports, m_result.reports);
        if (!er.words) {
            m_result.words.clear();
            return make_future_of(std::move(m_result));
        }
        join(*m_result.words, std::move(*er.words));
        return proceed();
    }

}; // class four_expansion_state

} // namespace

future<expansion_result> expand_four(
        const std::shared_ptr<world> &world,
        bool is_quoted,
        const std::shared_ptr<const word> &w) {
    auto es = std::make_shared<four_expansion_state>(
            world,
            is_quoted,
            std::shared_ptr<const components>(w, &w->components));
    return es->proceed();
}

} // namespace executing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
