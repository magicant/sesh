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

#include <utility>
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/assignment.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"
#include "language/syntax/raw_string.hh"
#include "language/syntax/simple_command.hh"
#include "language/syntax/word.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::assignment;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;
using sesh::language::syntax::raw_string;
using sesh::language::syntax::simple_command;
using sesh::language::syntax::word;

assignment::word_pointer new_word(xstring s) {
    assignment::word_pointer w(new word);
    w->add_component(word::component_pointer(new raw_string(s)));
    return w;
}

simple_command::assignment_pointer new_assignment(
        xstring name, xstring value) {
    return simple_command::assignment_pointer(
            new assignment(name, new_word(value)));
}

void add_assignment(simple_command &sc, xstring name, xstring value) {
    sc.assignments().push_back(new_assignment(name, value));
}

void add_word(simple_command &sc, xstring s) {
    sc.words().push_back(new_word(s));
}

TEST_CASE("Empty simple command print") {
    for_each_line_mode([](printer &p) {
        p << simple_command();
        CHECK(p.to_string() == L(""));
    });
}

TEST_CASE("Simple command print") {
    simple_command sc;

    add_assignment(sc, L("foo"), L("Foo.value"));
    add_word(sc, L("Hello"));
    add_assignment(sc, L("bar"), L("Bar-value"));
    add_word(sc, L("world"));

    for_each_line_mode([&sc](printer &p) {
        p << sc;
        CHECK(p.to_string() == L("foo=Foo.value bar=Bar-value Hello world"));
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
