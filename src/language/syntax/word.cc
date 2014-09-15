/* Copyright (C) 2013 WATANABE Yuki
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
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>
#include "common/maybe.hh"
#include "common/xstring.hh"

using sesh::common::make_maybe_of;
using sesh::common::maybe;
using sesh::common::xstring;

namespace sesh {
namespace language {
namespace syntax {

void word::add_component(component_pointer c) {
    m_maybe_constant_value_cache.clear();

    assert(c != nullptr);
    m_components.push_back(std::move(c));
}

void word::append(word &&w) {
    m_maybe_constant_value_cache.clear();
    w.m_maybe_constant_value_cache.clear();

    std::move(
            w.m_components.begin(),
            w.m_components.end(),
            std::back_inserter(m_components));
    w.m_components.clear();
}

maybe<xstring> word::compute_maybe_constant_value() const {
    xstring constantValue;
    for (const component_pointer &c : components())
        if (!c->append_constant_value(constantValue))
            return maybe<xstring>();
    return make_maybe_of(std::move(constantValue));
}

const maybe<xstring> &word::maybe_constant_value() const {
    if (!m_maybe_constant_value_cache)
        m_maybe_constant_value_cache.try_emplace(
                compute_maybe_constant_value());
    return m_maybe_constant_value_cache.value();
}

bool word::is_raw_string() const {
    return std::all_of(
            m_components.begin(),
            m_components.end(),
            [](const component_pointer &c) { return c->is_raw_string(); });
}

void word::print(printer &p) const {
    for (const component_pointer &c : components())
        p << *c;
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
