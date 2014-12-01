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
#include <utility>
#include "catch.hpp"
#include "common/visitor.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/printing/buffer.hh"
#include "language/printing/simple_command.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/simple_command_test_helper.hh"
#include "language/syntax/word.hh"
#include "language/syntax/word_test_helper.hh"

namespace {

using sesh::common::make_shared_visitable;
using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::make_simple_command_stub;
using sesh::language::syntax::make_word_stub;
using sesh::language::syntax::simple_command;
using sesh::language::syntax::word;

const xstring s1 = L("test"), s2 = L("case");
const xstring s = s1 + L(' ') + s2;

const simple_command single_word_command = make_simple_command_stub(s1);

const simple_command double_word_command = []() -> simple_command {
    simple_command c = single_word_command;
    c.words.push_back(make_word_stub(s2));
    return c;
}();

TEST_CASE("Printing single word command in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    print(single_word_command, b);
    CHECK(b.to_string() == s1);
}

namespace double_word_command_test {

template<buffer::line_mode_type mode>
struct print_double_word_command {
    buffer b{mode};
    print_double_word_command() {
        print(double_word_command, b);
    }
};

template<buffer::line_mode_type mode>
struct check_main_result : print_double_word_command<mode> {
    check_main_result() {
        CHECK(this->b.to_string() == s);
    }
};

TEST_CASE_METHOD(
        check_main_result<buffer::line_mode_type::single_line>,
        "Simple command prints space-separated words in single line mode") {
}

TEST_CASE_METHOD(
        check_main_result<buffer::line_mode_type::multi_line>,
        "Simple command prints space-separated words in multi-line mode") {
}

template<buffer::line_mode_type mode>
struct check_delayed_characters : print_double_word_command<mode> {
    check_delayed_characters() {
        this->b.append_main(L('!'));
        CHECK(this->b.to_string() == s + L(" !"));
    }
};

TEST_CASE_METHOD(
        check_delayed_characters<buffer::line_mode_type::single_line>,
        "Simple command leaves delayed space in single line mode") {
}

TEST_CASE_METHOD(
        check_delayed_characters<buffer::line_mode_type::multi_line>,
        "Simple command leaves delayed space in multi-line mode") {
}

TEST_CASE_METHOD(
        print_double_word_command<buffer::line_mode_type::single_line>,
        "Simple command leaves no delayed lines in single line mode") {
    this->b.break_line();
    CHECK(this->b.to_string() == s);
}

TEST_CASE_METHOD(
        print_double_word_command<buffer::line_mode_type::multi_line>,
        "Simple command leaves no delayed lines in multi-line mode") {
    this->b.break_line();
    CHECK(this->b.to_string() == s + L('\n'));
}

} // namespace double_word_command_test

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
