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

#ifndef INCLUDED_language_syntax_sequence_test_helper_hh
#define INCLUDED_language_syntax_sequence_test_helper_hh

#include "buildconfig.h"

#include <utility>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/xstring.hh"
#include "language/syntax/and_or_list_test_helper.hh"
#include "language/syntax/sequence.hh"

namespace sesh {
namespace language {
namespace syntax {

inline sequence make_sequence_stub(common::xstring &&s) {
    return {{make_and_or_list_stub(std::move(s))}};
}

inline sequence make_sequence_stub(const common::xstring &s) {
    return make_sequence_stub(common::copy(s));
}

inline void expect_raw_string_sequence(
        const sequence &actual_sequence,
        std::initializer_list<common::xstring> expected_words) {
    REQUIRE(actual_sequence.and_or_lists.size() == 1);
    const auto &a = actual_sequence.and_or_lists[0];
    expect_raw_string_and_or_list(a, expected_words);
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_sequence_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
