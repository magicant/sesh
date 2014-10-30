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
#include "format.hh"

#include <utility>
#include "boost/format.hpp"
#include "common/copy.hh"
#include "common/identity.hh"
#include "common/xchar.hh"

namespace sesh {
namespace ui {
namespace message {

namespace {

using sesh::common::copy;
using sesh::common::identity;

using char_type = format<>::char_type;

const char_type *default_string(const char_type *s) noexcept {
    return s == nullptr ? L("") : s;
}

} // namespace

format<>::format(string_type &&s) :
        m_format_string(std::move(s)), m_feed_arguments(identity()) { }

format<>::format(const string_type &s) : format(copy(s)) { }

format<>::format(const char_type *s) :
        format(string_type(default_string(s))) { }

auto format<>::to_string() const -> string_type {
    format_type f(m_format_string);
    m_feed_arguments(f);
    return f.str();
}

} // namespace message
} // namespace ui
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
