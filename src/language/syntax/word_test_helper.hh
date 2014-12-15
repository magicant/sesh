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

#ifndef INCLUDED_language_syntax_word_test_helper_hh
#define INCLUDED_language_syntax_word_test_helper_hh

#include "buildconfig.h"

#include <utility>
#include "catch.hpp"
#include "common/copy.hh"
#include "common/visitor.hh"
#include "common/visitor_test_helper.hh"
#include "common/xstring.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component_test_helper.hh"

namespace sesh {
namespace language {
namespace syntax {

inline word make_word_stub(common::xstring &&s) {
    word w;
    w.components.push_back(make_word_component_stub(std::move(s)));
    return w;
}

inline word make_word_stub(const common::xstring &s) {
    return make_word_stub(common::copy(s));
}

inline void expect_raw_string_word(
        const word &actual_word, const common::xstring &expected_string) {
    REQUIRE(actual_word.components.size() == 1);
    const auto &wc = actual_word.components[0];
    REQUIRE(wc != nullptr);
    auto checker = [&expected_string](const raw_string &rs) {
        CHECK(rs.value == expected_string);
    };
    visit(*wc, common::make_checking_visitor<raw_string>(checker));
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_word_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
