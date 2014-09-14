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

#include <functional>
#include "catch.hpp"
#include "common/xchar.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"

namespace {

using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::printer;

TEST_CASE("Initial printer yields empty string") {
    for_each_line_mode([](printer &p) {
        CHECK(p.to_string() == L(""));
    });
}

TEST_CASE("Printer << operator") {
    for_each_line_mode([](printer &p) {
        CHECK((p << 123).to_string() == L("123"));
    });
    for_each_line_mode([](printer &p) {
        CHECK((p << L('a')).to_string() == L("a"));
    });
    for_each_line_mode([](printer &p) {
        CHECK((p << 'a').to_string() == L("a"));
    });
    for_each_line_mode([](printer &p) {
        CHECK((p << L("abc")).to_string() == L("abc"));
    });
    for_each_line_mode([](printer &p) {
        CHECK((p << "abc").to_string() == L("abc"));
    });
}

TEST_CASE("Printer << operator concatenation") {
    for_each_line_mode([](printer &p) {
        p << L('1');
        CHECK(p.to_string() == L("1"));
        p << 2 << L("34");
        CHECK(p.to_string() == L("1234"));
    });
}

TEST_CASE("Printer delayed character buffer") {
    for_each_line_mode([](printer &p) {
        p << L('1');
        p.delayed_characters() << L('a');
        p << L('2');
        p.delayed_characters() << L('b') << L('c');
        p << L('3');
        p.delayed_characters() << L("ignored1");
        p.clear_delayed_characters();
        p << L('4');
        p.delayed_characters() << L("ignored2");
        p.clear_delayed_characters();
        p.delayed_characters() << L('d');
        p << L('5');
        CHECK(p.to_string() == L("1a2bc34d5"));
    });
}

TEST_CASE("Printer break_line single_line") {
    printer p(printer::line_mode_type::single_line);
    p << L('1');
    p.break_line();
    CHECK(p.to_string() == L("1"));
    p << L('2');
    CHECK(p.to_string() == L("1 2"));
    p.delayed_lines() << L("ignored1");
    CHECK(p.to_string() == L("1 2"));
    p << L('3');
    CHECK(p.to_string() == L("1 23"));
    p.break_line();
    CHECK(p.to_string() == L("1 23"));
    p << L('4');
    p.delayed_lines() << L("ignored2");
    p << L('5');
    p.delayed_lines() << L("ignored3");
    p << L('6');
    CHECK(p.to_string() == L("1 23 456"));
    p.break_line();
    p.break_line();
    p.delayed_characters() << L("ignored4");
    p.break_line();
    CHECK(p.to_string() == L("1 23 456"));
    p << L('7');
    CHECK(p.to_string() == L("1 23 456 7"));
}

TEST_CASE("Printer break_line multi_line") {
    printer p(printer::line_mode_type::multi_line);
    p << L('1');
    p.break_line();
    CHECK(p.to_string() == L("1\n"));
    p << L('2');
    CHECK(p.to_string() == L("1\n2"));
    p.delayed_lines() << L("R\n");
    CHECK(p.to_string() == L("1\n2"));
    p << L('3');
    CHECK(p.to_string() == L("1\n23"));
    p.break_line();
    CHECK(p.to_string() == L("1\n23\nR\n"));
    p << L('4');
    p.delayed_lines() << L("R1");
    p << L('5');
    p.delayed_lines() << L("R2");
    p << L('6');
    CHECK(p.to_string() == L("1\n23\nR\n456"));
    p.break_line();
    CHECK(p.to_string() == L("1\n23\nR\n456\nR1R2"));
    p.break_line();
    CHECK(p.to_string() == L("1\n23\nR\n456\nR1R2\n"));
}

TEST_CASE("Printer delayed characters and lines") {
    printer p(printer::line_mode_type::multi_line);
    p << L('1');
    p.delayed_characters() << L(';');
    p.break_line();
    CHECK(p.to_string() == L("1\n"));
    p << L('2');
    p.delayed_characters() << L(' ');
    p.delayed_lines() << L("R1\n");
    p.delayed_characters() << L(' ');
    CHECK(p.to_string() == L("1\n2"));
    p << L('3');
    CHECK(p.to_string() == L("1\n2  3"));
    p.delayed_characters() << L(' ');
    p.delayed_lines() << L("R2\n");
    p.delayed_characters() << L(' ');
    p.break_line();
    CHECK(p.to_string() == L("1\n2  3\nR1\nR2\n"));
}

TEST_CASE("Printer indentation single_line") {
    printer p(printer::line_mode_type::single_line);
    CHECK(p.indent_level() == 0);
    p.print_indent();
    CHECK(p.indent_level() == 0);
    CHECK(p.to_string() == L(""));
    p.indent_level() = 5;
    CHECK(p.indent_level() == 5);
    p.print_indent();
    CHECK(p.to_string() == L(""));
    p << L("foo");
    CHECK(p.indent_level() == 5);
    p.print_indent();
    CHECK(p.to_string() == L("foo"));
    p.indent_level() = 0;
    p.print_indent();
    CHECK(p.to_string() == L("foo"));
}

TEST_CASE("Printer indentation multi_line") {
    printer p(printer::line_mode_type::multi_line);
    p.print_indent();
    CHECK(p.indent_level() == 0);
    CHECK(p.to_string() == L(""));
    p.indent_level() = 2;
    CHECK(p.indent_level() == 2);
    p.print_indent();
    CHECK(p.to_string() == L("        "));
    p << L("foo");
    CHECK(p.indent_level() == 2);
    CHECK(p.to_string() == L("        foo"));
    p.indent_level() = 1;
    p.print_indent();
    CHECK(p.to_string() == L("        foo    "));
    p.print_indent();
    CHECK(p.to_string() == L("        foo        "));
    p << L("bar");
    p.indent_level() = 5;
    p.print_indent();
    CHECK(p.to_string() == L("        foo        bar                    "));
}

TEST_CASE("Printer indent guard") {
    for_each_line_mode([](printer &p) {
        CHECK(p.indent_level() == 0);
        {
            printer::indent_guard guard(p);
            CHECK(p.indent_level() == 1);
            {
                printer::indent_guard guard(p, 3);
                CHECK(p.indent_level() == 4);
            }
            CHECK(p.indent_level() == 1);
            {
                printer::indent_guard guard(p, 6);
                CHECK(p.indent_level() == 7);
                {
                    printer::indent_guard guard(p, 0);
                    CHECK(p.indent_level() == 7);
                }
                CHECK(p.indent_level() == 7);
            }
            CHECK(p.indent_level() == 1);
        }
        CHECK(p.indent_level() == 0);
    });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
