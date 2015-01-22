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

#ifndef INCLUDED_language_syntax_and_or_list_test_helper_hh
#define INCLUDED_language_syntax_and_or_list_test_helper_hh

#include "buildconfig.h"

#include <initializer_list>
#include <utility>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/xstring.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/pipeline_test_helper.hh"

namespace sesh {
namespace language {
namespace syntax {

inline and_or_list make_and_or_list_stub(common::xstring &&s) {
    and_or_list l;
    l.first = make_pipeline_stub(std::move(s));
    return l;
}

inline and_or_list make_and_or_list_stub(const common::xstring &s) {
    return make_and_or_list_stub(common::copy(s));
}

inline void expect_raw_string_and_or_list(
        const and_or_list &actual_list,
        std::initializer_list<common::xstring> expected_words) {
    CHECK(actual_list.synchronicity ==
            and_or_list::synchronicity_type::sequential);
    CHECK(actual_list.rest.empty());
    expect_raw_string_pipeline(actual_list.first, expected_words);
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_and_or_list_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
