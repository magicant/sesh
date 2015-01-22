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

#ifndef INCLUDED_language_syntax_pipeline_test_helper_hh
#define INCLUDED_language_syntax_pipeline_test_helper_hh

#include "buildconfig.h"

#include <initializer_list>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/xstring.hh"
#include "language/syntax/command_test_helper.hh"
#include "language/syntax/pipeline.hh"

namespace sesh {
namespace language {
namespace syntax {

inline pipeline make_pipeline_stub(common::xstring &&s) {
    pipeline p;
    p.commands.push_back(make_command_stub(s));
    return p;
}

inline pipeline make_pipeline_stub(const common::xstring &s) {
    return make_pipeline_stub(common::copy(s));
}

inline void expect_raw_string_pipeline(
        const pipeline &actual_pipeline,
        std::initializer_list<common::xstring> expected_words) {
    CHECK(actual_pipeline.exit_status_mode ==
            pipeline::exit_status_mode_type::straight);
    REQUIRE(actual_pipeline.commands.size() == 1);
    const auto &command_ptr = actual_pipeline.commands[0];
    REQUIRE(command_ptr != nullptr);
    expect_raw_string_command(*command_ptr, expected_words);
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_pipeline_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
