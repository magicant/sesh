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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common.hh"
#include <functional>
#include "language/syntax/Printer.hh"
#include "language/syntax/PrinterTestHelper.hh"

using sesh::language::syntax::Printer;
using sesh::language::syntax::forEachLineMode;

TEST_CASE("Initial printer yields empty string") {
    forEachLineMode([](Printer &p) {
        CHECK(p.toWstring() == L"");
    });
}

TEST_CASE("Printer << operator") {
    forEachLineMode([](Printer &p) {
        CHECK((p << 123).toWstring() == L"123");
    });
    forEachLineMode([](Printer &p) {
        CHECK((p << L'a').toWstring() == L"a");
    });
    forEachLineMode([](Printer &p) {
        CHECK((p << 'a').toWstring() == L"a");
    });
    forEachLineMode([](Printer &p) {
        CHECK((p << L"abc").toWstring() == L"abc");
    });
    forEachLineMode([](Printer &p) {
        CHECK((p << "abc").toWstring() == L"abc");
    });
}

TEST_CASE("Printer << operator concatenation") {
    forEachLineMode([](Printer &p) {
        p << L'1';
        CHECK(p.toWstring() == L"1");
        p << 2 << L"34";
        CHECK(p.toWstring() == L"1234");
    });
}

TEST_CASE("Printer delayed character buffer") {
    forEachLineMode([](Printer &p) {
        p << L'1';
        p.delayedCharacters() << L'a';
        p << L'2';
        p.delayedCharacters() << L'b' << L'c';
        p << L'3';
        p.delayedCharacters() << L"ignored1";
        p.clearDelayedCharacters();
        p << L'4';
        p.delayedCharacters() << L"ignored2";
        p.clearDelayedCharacters();
        p.delayedCharacters() << L'd';
        p << L'5';
        CHECK(p.toWstring() == L"1a2bc34d5");
    });
}

TEST_CASE("Printer breakLine SINGLE_LINE") {
    Printer p(Printer::LineMode::SINGLE_LINE);
    p << L'1';
    p.breakLine();
    CHECK(p.toWstring() == L"1");
    p << L'2';
    CHECK(p.toWstring() == L"1 2");
    p.delayedLines() << L"ignored1";
    CHECK(p.toWstring() == L"1 2");
    p << L'3';
    CHECK(p.toWstring() == L"1 23");
    p.breakLine();
    CHECK(p.toWstring() == L"1 23");
    p << L'4';
    p.delayedLines() << L"ignored2";
    p << L'5';
    p.delayedLines() << L"ignored3";
    p << L'6';
    CHECK(p.toWstring() == L"1 23 456");
    p.breakLine();
    p.breakLine();
    p.delayedCharacters() << L"ignored4";
    p.breakLine();
    CHECK(p.toWstring() == L"1 23 456");
    p << L'7';
    CHECK(p.toWstring() == L"1 23 456 7");
}

TEST_CASE("Printer breakLine MULTI_LINE") {
    Printer p(Printer::LineMode::MULTI_LINE);
    p << L'1';
    p.breakLine();
    CHECK(p.toWstring() == L"1\n");
    p << L'2';
    CHECK(p.toWstring() == L"1\n2");
    p.delayedLines() << L"R\n";
    CHECK(p.toWstring() == L"1\n2");
    p << L'3';
    CHECK(p.toWstring() == L"1\n23");
    p.breakLine();
    CHECK(p.toWstring() == L"1\n23\nR\n");
    p << L'4';
    p.delayedLines() << L"R1";
    p << L'5';
    p.delayedLines() << L"R2";
    p << L'6';
    CHECK(p.toWstring() == L"1\n23\nR\n456");
    p.breakLine();
    CHECK(p.toWstring() == L"1\n23\nR\n456\nR1R2");
    p.breakLine();
    CHECK(p.toWstring() == L"1\n23\nR\n456\nR1R2\n");
}

TEST_CASE("Printer delayed characters and lines") {
    Printer p(Printer::LineMode::MULTI_LINE);
    p << L'1';
    p.delayedCharacters() << L';';
    p.breakLine();
    CHECK(p.toWstring() == L"1\n");
    p << L'2';
    p.delayedCharacters() << L' ';
    p.delayedLines() << L"R1\n";
    p.delayedCharacters() << L' ';
    CHECK(p.toWstring() == L"1\n2");
    p << L'3';
    CHECK(p.toWstring() == L"1\n2  3");
    p.delayedCharacters() << L' ';
    p.delayedLines() << L"R2\n";
    p.delayedCharacters() << L' ';
    p.breakLine();
    CHECK(p.toWstring() == L"1\n2  3\nR1\nR2\n");
}

TEST_CASE("Printer indentation SINGLE_LINE") {
    Printer p(Printer::LineMode::SINGLE_LINE);
    CHECK(p.indentLevel() == 0);
    p.printIndent();
    CHECK(p.indentLevel() == 0);
    CHECK(p.toWstring() == L"");
    p.indentLevel() = 5;
    CHECK(p.indentLevel() == 5);
    p.printIndent();
    CHECK(p.toWstring() == L"");
    p << L"foo";
    CHECK(p.indentLevel() == 5);
    p.printIndent();
    CHECK(p.toWstring() == L"foo");
    p.indentLevel() = 0;
    p.printIndent();
    CHECK(p.toWstring() == L"foo");
}

TEST_CASE("Printer indentation MULTI_LINE") {
    Printer p(Printer::LineMode::MULTI_LINE);
    p.printIndent();
    CHECK(p.indentLevel() == 0);
    CHECK(p.toWstring() == L"");
    p.indentLevel() = 2;
    CHECK(p.indentLevel() == 2);
    p.printIndent();
    CHECK(p.toWstring() == L"        ");
    p << L"foo";
    CHECK(p.indentLevel() == 2);
    CHECK(p.toWstring() == L"        foo");
    p.indentLevel() = 1;
    p.printIndent();
    CHECK(p.toWstring() == L"        foo    ");
    p.printIndent();
    CHECK(p.toWstring() == L"        foo        ");
    p << L"bar";
    p.indentLevel() = 5;
    p.printIndent();
    CHECK(p.toWstring() == L"        foo        bar                    ");
}

TEST_CASE("Printer indent guard") {
    forEachLineMode([](Printer &p) {
        CHECK(p.indentLevel() == 0);
        {
            Printer::IndentGuard guard(p);
            CHECK(p.indentLevel() == 1);
            {
                Printer::IndentGuard guard(p, 3);
                CHECK(p.indentLevel() == 4);
            }
            CHECK(p.indentLevel() == 1);
            {
                Printer::IndentGuard guard(p, 6);
                CHECK(p.indentLevel() == 7);
                {
                    Printer::IndentGuard guard(p, 0);
                    CHECK(p.indentLevel() == 7);
                }
                CHECK(p.indentLevel() == 7);
            }
            CHECK(p.indentLevel() == 1);
        }
        CHECK(p.indentLevel() == 0);
    });
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
