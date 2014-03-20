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
#include "DiagnosticEnvironment.hh"

#include <utility>
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "language/source/DiagnosticMessage.hh"

using sesh::common::ErrorLevel;
using sesh::common::Message;
using sesh::language::source::DiagnosticMessage;

namespace sesh {
namespace language {
namespace parser {

void DiagnosticEnvironment::addDiagnosticMessage(
        Size position, Message<> &&m, ErrorLevel el) {
    mDiagnosticMessages.emplace_back(
            DiagnosticMessage::Position(buffer().shared_from_this(), position),
            std::move(m),
            el);
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
