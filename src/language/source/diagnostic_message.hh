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

#ifndef INCLUDED_language_source_diagnostic_message_hh
#define INCLUDED_language_source_diagnostic_message_hh

#include "buildconfig.h"

#include "common/error_level.hh"
#include "common/message.hh"
#include "language/source/buffer.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * A diagnostic message is a message associated with a position in a source
 * buffer and an error level.
 */
class diagnostic_message {

public:

    using position_type = buffer::const_iterator;

private:

    position_type m_position;
    common::error_level m_error_level;
    common::message<> m_message;

public:

    diagnostic_message(
            position_type, common::message<> &&, common::error_level);

    const position_type &position() const noexcept { return m_position; }
    common::error_level error_level() const noexcept { return m_error_level; }
    const common::message<> &message() const noexcept { return m_message; }
    common::message<> &message() noexcept { return m_message; }

}; // class diagnostic_message

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_diagnostic_message_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
