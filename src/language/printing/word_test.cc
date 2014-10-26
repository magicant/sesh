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

#include <memory>
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/printing/buffer.hh"
#include "language/printing/word.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_component.hh"
#include "language/syntax/word_component_test_helper.hh"
#include "language/syntax/word_test_helper.hh"

namespace {

using sesh::common::make_shared_visitable;
using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::make_word_component_stub;
using sesh::language::syntax::make_word_stub;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;
using sesh::language::syntax::word_component;

TEST_CASE("Printing single-component word in single line mode") {
    xstring s = L("test");
    auto wc = make_shared_visitable<word_component>(raw_string{s});
    word w = make_word_stub(s);
    buffer b(buffer::line_mode_type::single_line);
    print(w, b);
    CHECK(b.to_string() == s);
}

namespace multi_component_word_test {

template<buffer::line_mode_type mode>
struct print_two_words {

    word w;
    buffer b{mode};
    xstring s1 = L("test"), s2 = L("case"), s = s1 + s2;

    print_two_words() {
        w.components.push_back(make_word_component_stub(s1));
        w.components.push_back(make_word_component_stub(s2));
        print(w, b);
    }

}; // struct print_two_words

template<buffer::line_mode_type mode>
struct print_two_words_and_check_main_result : print_two_words<mode> {
    print_two_words_and_check_main_result() {
        CHECK(this->b.to_string() == this->s);
    }
};

TEST_CASE_METHOD(
        print_two_words_and_check_main_result<
                buffer::line_mode_type::single_line>,
        "Printing multi-component word in single line mode") {
}

TEST_CASE_METHOD(
        print_two_words_and_check_main_result<
                buffer::line_mode_type::multi_line>,
        "Printing multi-component word in multi-line mode") {
}

template<buffer::line_mode_type mode>
struct check_delayed_characters : print_two_words<mode> {
    check_delayed_characters() {
        this->b.append_main(L('!'));
        CHECK(this->b.to_string() == this->s + L(" !"));
    }
};

TEST_CASE_METHOD(
        check_delayed_characters<buffer::line_mode_type::single_line>,
        "Word leaves delayed space in single line mode") {
}

TEST_CASE_METHOD(
        check_delayed_characters<buffer::line_mode_type::multi_line>,
        "Word leaves delayed space in multi-line mode") {
}

TEST_CASE_METHOD(
        print_two_words<buffer::line_mode_type::single_line>,
        "Word leaves no delayed lines in single line mode") {
    this->b.break_line();
    CHECK(this->b.to_string() == this->s);
}

TEST_CASE_METHOD(
        print_two_words<buffer::line_mode_type::multi_line>,
        "Word leaves no delayed lines in multi-line mode") {
    this->b.break_line();
    CHECK(this->b.to_string() == this->s + L("\n"));
}

} // namespace multi_component_word_test

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
