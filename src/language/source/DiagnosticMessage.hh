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

#ifndef INCLUDED_language_source_DiagnosticMessage_hh
#define INCLUDED_language_source_DiagnosticMessage_hh

#include "buildconfig.h"

#include "common/Message.hh"
#include "common/error_level.hh"
#include "language/source/Buffer.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * A diagnostic message is a message associated with a position in a source
 * buffer and an error level.
 */
class DiagnosticMessage {

public:

    using Position = source::Buffer::ConstIterator;

private:

    Position mPosition;
    common::error_level mErrorLevel;
    common::Message<> mMessage;

public:

    DiagnosticMessage(Position, common::Message<> &&, common::error_level);

    const Position &position() const noexcept { return mPosition; }
    common::error_level errorLevel() const noexcept { return mErrorLevel; }
    const common::Message<> &message() const noexcept { return mMessage; }
    common::Message<> &message() noexcept { return mMessage; }

}; // class DiagnosticMessage

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_DiagnosticMessage_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
