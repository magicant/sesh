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

#ifndef INCLUDED_language_syntax_command_test_helper_hh
#define INCLUDED_language_syntax_command_test_helper_hh

#include "buildconfig.h"

#include <initializer_list>
#include "common/copy.hh"
#include "common/visitor.hh"
#include "common/visitor_test_helper.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/simple_command_test_helper.hh"

namespace sesh {
namespace language {
namespace syntax {

inline auto make_command_stub(common::xstring &&s)
        -> std::shared_ptr<const command> {
    return common::make_shared_visitable<command>(make_simple_command_stub(s));
}

inline auto make_command_stub(const common::xstring &s)
        -> std::shared_ptr<const command> {
    return make_command_stub(common::copy(s));
}

inline void expect_raw_string_command(
        const command &actual_command,
        std::initializer_list<common::xstring> expected_words) {
    const auto check = [expected_words](const simple_command &sc) {
        expect_raw_string_simple_command(sc, expected_words);
    };
    visit(
            actual_command,
            common::make_checking_visitor<simple_command>(check));
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_command_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
