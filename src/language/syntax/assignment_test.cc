/* Copyright (C) 2013 WATANABE Yuki
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

#include <cstddef>
#include <stdexcept>
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/assignment.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/word.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::assignment;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::word;

struct fixture {
    assignment a;
    fixture() : a() {
        a.variable_name() = L("varName");
        a.value().add_component(word::component_pointer(
                new raw_string(L("assigned"))));
        a.value().add_component(word::component_pointer(
                new raw_string(L("Value"))));
    }
};

TEST_CASE("Assignment constructors") {
    assignment a1;
    CHECK(a1.variable_name() == L(""));
    CHECK(a1.value().components().size() == 0);

    xstring name(L("name"));
    assignment a2(name, nullptr);
    CHECK(a2.variable_name() == name);
    CHECK(a2.value().components().size() == 0);

    assignment a3(xstring(name), nullptr);
    CHECK(a3.variable_name() == name);
    CHECK(a3.value().components().size() == 0);

    assignment a4(name, assignment::word_pointer(new word));
    CHECK(a4.variable_name() == name);
    CHECK(a4.value().components().size() == 0);

    assignment a5(xstring(name), assignment::word_pointer(new word));
    CHECK(a5.variable_name() == name);
    CHECK(a5.value().components().size() == 0);
}

TEST_CASE_METHOD(fixture, "Assignment data 1") {
    CHECK(a.variable_name() == L("varName"));
}

TEST_CASE("Assignment data 2") {
    xstring name(L("name"));
    assignment a1(name, assignment::word_pointer(new word));
    a1.value().add_component(
            word::component_pointer(new raw_string(L("value"))));
    a1.value().add_component(
            word::component_pointer(new raw_string(L(" "))));
    a1.value().add_component(
            word::component_pointer(new raw_string(L("string"))));
    CHECK(a1.variable_name() == name);
    CHECK(a1.value().components().size() == 3);

    assignment a2(xstring(name), assignment::word_pointer(new word));
    CHECK(a2.variable_name() == name);
    CHECK(a2.value().components().size() == 0);
}

TEST_CASE_METHOD(fixture, "Assignment print") {
    for_each_line_mode([this](printer &p) {
        p << a;
        CHECK(p.to_string() == L("varName=assignedValue"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
