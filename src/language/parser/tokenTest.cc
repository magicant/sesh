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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/token.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::common::CharTraits;
using sesh::common::String;
using sesh::language::parser::BasicEnvironmentStub;
using sesh::language::parser::NeedMoreSource;
using sesh::language::parser::Environment;
using sesh::language::parser::parseSymbol;
using sesh::language::source::SourceBuffer;

using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

#define CHECK_TOKEN(f, s) do { INFO(s); f(L(s)); } while (0)

void checkNonSymbol(String &&s) {
    BasicEnvironmentStub e;
    e.appendSource(std::move(s));
    CHECK(peekSymbol(e) == String());
    CHECK(e.current() == e.begin());
    CHECK(parseSymbol(e) == String());
    CHECK(e.current() == e.begin());
}

TEST_CASE("Token, check non symbols") {
    CHECK_TOKEN(checkNonSymbol, "!");
    CHECK_TOKEN(checkNonSymbol, "\"");
    CHECK_TOKEN(checkNonSymbol, "#");
    CHECK_TOKEN(checkNonSymbol, "$");
    CHECK_TOKEN(checkNonSymbol, "%");
    CHECK_TOKEN(checkNonSymbol, "'");
    CHECK_TOKEN(checkNonSymbol, "*");
    CHECK_TOKEN(checkNonSymbol, "+");
    CHECK_TOKEN(checkNonSymbol, ",");
    CHECK_TOKEN(checkNonSymbol, "-");
    CHECK_TOKEN(checkNonSymbol, ".");
    CHECK_TOKEN(checkNonSymbol, "/");
    CHECK_TOKEN(checkNonSymbol, ":");
    CHECK_TOKEN(checkNonSymbol, "=");
    CHECK_TOKEN(checkNonSymbol, "?");
    CHECK_TOKEN(checkNonSymbol, "@");
    CHECK_TOKEN(checkNonSymbol, "[");
    CHECK_TOKEN(checkNonSymbol, "]");
    CHECK_TOKEN(checkNonSymbol, "^");
    CHECK_TOKEN(checkNonSymbol, "_");
    CHECK_TOKEN(checkNonSymbol, "`");
    CHECK_TOKEN(checkNonSymbol, "{");
    CHECK_TOKEN(checkNonSymbol, "}");
    CHECK_TOKEN(checkNonSymbol, "~");

    CHECK_TOKEN(checkNonSymbol, "\n");
    CHECK_TOKEN(checkNonSymbol, "\t");
    CHECK_TOKEN(checkNonSymbol, " ");
    CHECK_TOKEN(checkNonSymbol, "A");
    CHECK_TOKEN(checkNonSymbol, "a");
    CHECK_TOKEN(checkNonSymbol, "0");

    CHECK_TOKEN(checkNonSymbol, "\\\\");
    CHECK_TOKEN(checkNonSymbol, "\\X");
}

void checkSymbolNeedMoreSource(String &&s) {
    BasicEnvironmentStub e;
    e.appendSource(std::move(s));
    CHECK_THROWS_AS(peekSymbol(e), NeedMoreSource);
    CHECK_THROWS_AS(parseSymbol(e), NeedMoreSource);
}

TEST_CASE("Token, check symbol need more source") {
    CHECK_TOKEN(checkSymbolNeedMoreSource, "");
    CHECK_TOKEN(checkSymbolNeedMoreSource, "&");
    CHECK_TOKEN(checkSymbolNeedMoreSource, "|");
    CHECK_TOKEN(checkSymbolNeedMoreSource, ";");
    CHECK_TOKEN(checkSymbolNeedMoreSource, "<");
    CHECK_TOKEN(checkSymbolNeedMoreSource, ">");
}

void checkSymbolWithEof(const String &s) {
    BasicEnvironmentStub e;
    e.appendSource(String(s));

    e.setIsEof(true);
    CHECK(peekSymbol(e) == s);
    CHECK(e.current() == e.begin());
    CHECK(parseSymbol(e) == s);
    CHECK(e.current() == e.begin() + s.length());

    e.current() = e.begin();
    e.setIsEof(false);
    e.appendSource(L(" "));
    CHECK(peekSymbol(e) == s);
    CHECK(e.current() == e.begin());
    CHECK(parseSymbol(e) == s);
    CHECK(e.current() == e.begin() + s.length());
}

TEST_CASE("Token, check symbol with EOF") {
    CHECK_TOKEN(checkSymbolWithEof, "&");
    CHECK_TOKEN(checkSymbolWithEof, "|");
    CHECK_TOKEN(checkSymbolWithEof, ";");
    CHECK_TOKEN(checkSymbolWithEof, "<");
    CHECK_TOKEN(checkSymbolWithEof, "<<");
    CHECK_TOKEN(checkSymbolWithEof, ">");
}

void checkSymbolWithoutEof(const String &s) {
    BasicEnvironmentStub e;
    e.appendSource(String(s));
    CHECK(peekSymbol(e) == s);
    CHECK(e.current() == e.begin());
    CHECK(parseSymbol(e) == s);
    CHECK(e.current() == e.begin() + s.length());
}

TEST_CASE("Token, check symbol without EOF") {
    CHECK_TOKEN(checkSymbolWithoutEof, "&&");
    CHECK_TOKEN(checkSymbolWithoutEof, "||");
    CHECK_TOKEN(checkSymbolWithoutEof, ";;");
    CHECK_TOKEN(checkSymbolWithoutEof, "<<-");
    CHECK_TOKEN(checkSymbolWithoutEof, "<&");
    CHECK_TOKEN(checkSymbolWithoutEof, "<|");
    CHECK_TOKEN(checkSymbolWithoutEof, "<>");
    CHECK_TOKEN(checkSymbolWithoutEof, ">>");
    CHECK_TOKEN(checkSymbolWithoutEof, ">&");
    CHECK_TOKEN(checkSymbolWithoutEof, ">|");
    CHECK_TOKEN(checkSymbolWithoutEof, "(");
    CHECK_TOKEN(checkSymbolWithoutEof, ")");
}

TEST_CASE("Token, symbol line continuations") {
    CHECK_TOKEN(checkNonSymbol, "\\\n~");
    CHECK_TOKEN(checkNonSymbol, "\\\n\\X");
    CHECK_TOKEN(checkSymbolNeedMoreSource, "\\\n>");

    {
        BasicEnvironmentStub e;
        e.appendSource(L("\\\n\\\n>"));

        e.setIsEof(true);
        CHECK(peekSymbol(e) == L(">"));
        CHECK(e.current() == e.begin());
        CHECK(parseSymbol(e) == L(">"));
        CHECK(e.current() == e.begin() + 1);

        e.current() = e.begin();
        e.setIsEof(false);
        e.appendSource(L(" "));
        CHECK(parseSymbol(e) == L(">"));
        CHECK(e.current() == e.begin() + 1);
    }
    {
        BasicEnvironmentStub e;
        e.appendSource(L("\\\n\\\n<\\\n&"));
        CHECK(peekSymbol(e) == L("<&"));
        CHECK(e.current() == e.begin());
        CHECK(parseSymbol(e) == L("<&"));
        CHECK(e.current() == e.begin() + 2);
    }
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
