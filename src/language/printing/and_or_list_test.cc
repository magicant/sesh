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
#include "language/printing/and_or_list.hh"
#include "language/printing/buffer.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/and_or_list_test_helper.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/conditional_pipeline_test_helper.hh"

namespace {

using sesh::common::xstring;
using sesh::language::printing::buffer;
using sesh::language::syntax::and_or_list;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::make_and_or_list_stub;
using sesh::language::syntax::make_conditional_pipeline_stub;

const xstring s1 = L("test"), s2 = L("command");

TEST_CASE("Printing sequential and-or list in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    print(l, b);
    CHECK(b.to_string() == s1);
}

TEST_CASE("Sequential and-or list leaves delayed semicolon") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    print(l, b);
    b.commit_delayed_characters();
    CHECK(b.to_string() == s1 + L("; "));
}

TEST_CASE("Printing sequential and-or list in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    auto l = make_and_or_list_stub(s2);
    print(l, b);
    CHECK(b.to_string() == s2);
}

TEST_CASE("Printing asynchronous and-or list in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    l.synchronicity = and_or_list::synchronicity_type::asynchronous;
    print(l, b);
    CHECK(b.to_string() == s1 + L('&'));
}

TEST_CASE("Asynchronous and-or list leaves delayed space") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    l.synchronicity = and_or_list::synchronicity_type::asynchronous;
    print(l, b);
    b.commit_delayed_characters();
    CHECK(b.to_string() == s1 + L("& "));
}

TEST_CASE(
        "Printing sequential and-or list with a conditional pipeline "
        "in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    l.rest.push_back(make_conditional_pipeline_stub(s2));
    print(l, b);
    CHECK(b.to_string() == s1 + L(" && ") + s2);
}

TEST_CASE(
        "Printing asynchronous and-or list with a conditional pipeline "
        "in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    auto l = make_and_or_list_stub(s1);
    l.rest.push_back(make_conditional_pipeline_stub(s2));
    l.synchronicity = and_or_list::synchronicity_type::asynchronous;
    print(l, b);
    CHECK(b.to_string() == s1 + L(" && ") + s2 + L('&'));
}

TEST_CASE(
        "Printing asynchronous and-or list with a conditional pipeline "
        "in multi-line mode") {
    buffer b(buffer::line_mode_type::multi_line);
    auto l = make_and_or_list_stub(s1);
    l.rest.push_back(make_conditional_pipeline_stub(s2));
    l.synchronicity = and_or_list::synchronicity_type::asynchronous;
    print(l, b);
    CHECK(b.to_string() == s1 + L(" &&\n") + s2 + L('&'));
}

TEST_CASE(
        "Printing sequential and-or list with 3 conditional pipelines "
        "in single line mode") {
    buffer b(buffer::line_mode_type::single_line);
    const xstring s3 = L("continue"), s4 = L("more");
    auto l = make_and_or_list_stub(s1);
    l.rest.push_back(make_conditional_pipeline_stub(s2));
    l.rest.push_back(make_conditional_pipeline_stub(s3));
    l.rest.push_back(make_conditional_pipeline_stub(s4));
    l.rest[1].condition = conditional_pipeline::condition_type::or_else;
    print(l, b);
    CHECK(b.to_string() ==
            s1 + L(" && ") + s2 + L(" || ") + s3 + L(" && ") + s4);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
