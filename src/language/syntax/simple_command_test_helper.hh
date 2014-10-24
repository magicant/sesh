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

#ifndef INCLUDED_language_syntax_simple_command_test_helper_hh
#define INCLUDED_language_syntax_simple_command_test_helper_hh

#include "buildconfig.h"

#include "common/copy.hh"
#include "common/xstring.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/word_test_helper.hh"

namespace sesh {
namespace language {
namespace syntax {

simple_command make_simple_command_stub(common::xstring &&s) {
    simple_command c;
    c.words.push_back(make_word_stub(s));
    return c;
}

simple_command make_simple_command_stub(const common::xstring &s) {
    return make_simple_command_stub(common::copy(s));
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_simple_command_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
