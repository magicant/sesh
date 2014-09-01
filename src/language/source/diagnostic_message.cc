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
#include "diagnostic_message.hh"

#include <utility>
#include "common/error_level.hh"
#include "common/message.hh"

using sesh::common::error_level;

namespace sesh {
namespace language {
namespace source {

diagnostic_message::diagnostic_message(
        position_type p, common::message<> &&m, common::error_level el) :
        m_position(std::move(p)),
        m_error_level(el),
        m_message(std::move(m)) { }

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
