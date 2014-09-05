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

#ifndef INCLUDED_language_source_diagnostic_message_test_helper_hh
#define INCLUDED_language_source_diagnostic_message_test_helper_hh

#include "buildconfig.h"

#include "common/message.hh"
#include "language/source/diagnostic_message.hh"

namespace sesh {
namespace language {
namespace source {

void checkEqual(const diagnostic_message &a, const diagnostic_message &b) {
    CHECK(a.position() == b.position());
    CHECK(a.error_level() == b.error_level());
    CHECK(a.message().toString() == b.message().toString());
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_diagnostic_message_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
