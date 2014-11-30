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

#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/printing/buffer.hh"
#include "language/printing/pipeline.hh"
#include "language/syntax/command_test_helper.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/pipeline_test_helper.hh"

namespace {

using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::make_command_stub;
using sesh::language::syntax::make_pipeline_stub;
using sesh::language::syntax::pipeline;

const xstring s1 = L("test"), s2 = L("command"), s3 = L("name");
const xstring separator = L(" | ");

const pipeline single_command_pipeline = make_pipeline_stub(s1);

const pipeline double_command_pipeline = []() -> pipeline {
    pipeline p = single_command_pipeline;
    p.commands.push_back(make_command_stub(s2));
    return p;
}();

const pipeline triple_command_pipeline = []() -> pipeline {
    pipeline p = double_command_pipeline;
    p.commands.push_back(make_command_stub(s3));
    return p;
}();

const pipeline negated_pipeline = []() -> pipeline {
    pipeline p = single_command_pipeline;
    p.exit_status_mode = pipeline::exit_status_mode_type::negated;
    return p;
}();

// XXX GCC 4.8 doesn't seem to support reference template parameters.
// template<buffer::line_mode_type mode, const pipeline &p>
template<buffer::line_mode_type mode, const pipeline *p>
struct print_pipeline {
    buffer b{mode};
    print_pipeline() {
        print(*p, b);
    }
};

constexpr auto S = buffer::line_mode_type::single_line;
constexpr auto M = buffer::line_mode_type::multi_line;

TEST_CASE("Printing single command pipeline in single line mode") {
    print_pipeline<S, &single_command_pipeline> p;
    CHECK(p.b.to_string() == s1);
}

namespace double_command_pipeline_test {

template<buffer::line_mode_type mode>
struct check_main_result : print_pipeline<mode, &double_command_pipeline> {
    check_main_result() {
        CHECK(this->b.to_string() == s1 + separator + s2);
    }
};

TEST_CASE_METHOD(
    check_main_result<S>,
    "Double command pipeline prints commands separated by vertical bar "
    "in single line mode") {
}

TEST_CASE_METHOD(
    check_main_result<M>,
    "Double command pipeline prints commands separated by vertical bar "
    "in multi-line mode") {
}

} // namespace double_command_pipeline_test

namespace triple_command_pipeline_test {

template<buffer::line_mode_type mode>
struct check_main_result : print_pipeline<mode, &triple_command_pipeline> {
    check_main_result() {
        CHECK(this->b.to_string() == s1 + separator + s2 + separator + s3);
    }
};

TEST_CASE_METHOD(
    check_main_result<S>,
    "Triple command pipeline prints commands separated by vertical bar "
    "in single line mode") {
}

TEST_CASE_METHOD(
    check_main_result<M>,
    "Triple command pipeline prints commands separated by vertical bar "
    "in multi-line mode") {
}

} // namespace triple_command_pipeline_test

namespace negated_pipeline_test {

template<buffer::line_mode_type mode>
struct check_main_result : print_pipeline<mode, &negated_pipeline> {
    check_main_result() {
        CHECK(this->b.to_string() == L("! ") + s1);
    }
};

TEST_CASE_METHOD(
    check_main_result<S>,
    "Negated pipeline prints commands preceded by exclamation "
    "in single line mode") {
}

TEST_CASE_METHOD(
    check_main_result<M>,
    "Negated pipeline prints commands preceded by exclamation "
    "in multi-line mode") {
}

} // negated_pipeline_test {

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
