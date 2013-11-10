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
using sesh::language::parser::CLocaleEnvironmentStub;
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

void checkNonKeywordWithoutEof(String &&s) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(s));
    CHECK(peekKeyword(e) == String());
    CHECK(e.current() == e.begin());
    CHECK(parseKeyword(e) == String());
    CHECK(e.current() == e.begin());
}

TEST_CASE("Token, check non keywords without EOF") {
    CHECK_TOKEN(checkNonKeywordWithoutEof, "\"");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "#");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "$");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "%");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "&");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "'");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "(");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ")");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "*");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "+");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ",");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "-");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ".");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "/");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ":");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ";");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "<");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "=");
    CHECK_TOKEN(checkNonKeywordWithoutEof, ">");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "?");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "@");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "^");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "_");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "`");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "|");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "~");

    CHECK_TOKEN(checkNonKeywordWithoutEof, "\n");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "\t");
    CHECK_TOKEN(checkNonKeywordWithoutEof, " ");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "A");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "a");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "0");

    CHECK_TOKEN(checkNonKeywordWithoutEof, "\\\\");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "\\X");

    CHECK_TOKEN(checkNonKeywordWithoutEof, "!!");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "!X");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "[[[");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "[[X");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "]]]");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "]]X");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "{{");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "{X");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "}}");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "}X");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "case!");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "caseX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "casex");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "cX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "caX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "casX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "dX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "doX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "donX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "doneX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "eX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "elX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "eliX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "elifX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "elsX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "elseX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "esX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "esaX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "esacX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "fX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "fiX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "forX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "fuX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "funX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "funcX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "functX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "functiX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "functioX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "iX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "ifX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "inX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "sX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "seX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "selX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "seleX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "selecX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "selectX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "tX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "thX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "theX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "thenX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "uX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "unX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "untX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "untiX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "untilX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "wX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "whX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "whiX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "whilX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "whileX");

    CHECK_TOKEN(checkNonKeywordWithoutEof, "\\\nc\\\n\\\nase\\\nX");
    CHECK_TOKEN(checkNonKeywordWithoutEof, "\\\nwh\\\n\\\nile\\\nX");
}

void checkNonKeywordWithEof(String &&s) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(s));
    e.setIsEof();
    CHECK(peekKeyword(e) == String());
    CHECK(e.current() == e.begin());
    CHECK(parseKeyword(e) == String());
    CHECK(e.current() == e.begin());
}

void checkKeywordNeedMoreSource(String &&s) {
    CLocaleEnvironmentStub e;
    e.appendSource(std::move(s));
    CHECK_THROWS_AS(peekKeyword(e), NeedMoreSource);
    CHECK_THROWS_AS(parseKeyword(e), NeedMoreSource);
}

void checkKeywordPrefix(const String &s) {
    checkKeywordNeedMoreSource(String(s));
    checkNonKeywordWithEof(String(s));
}

TEST_CASE("Token, check keyword prefixes") {
    CHECK_TOKEN(checkKeywordPrefix, "");
    CHECK_TOKEN(checkKeywordPrefix, "c");
    CHECK_TOKEN(checkKeywordPrefix, "ca");
    CHECK_TOKEN(checkKeywordPrefix, "cas");
    CHECK_TOKEN(checkKeywordPrefix, "d");
    CHECK_TOKEN(checkKeywordPrefix, "don");
    CHECK_TOKEN(checkKeywordPrefix, "e");
    CHECK_TOKEN(checkKeywordPrefix, "el");
    CHECK_TOKEN(checkKeywordPrefix, "eli");
    CHECK_TOKEN(checkKeywordPrefix, "els");
    CHECK_TOKEN(checkKeywordPrefix, "es");
    CHECK_TOKEN(checkKeywordPrefix, "esa");
    CHECK_TOKEN(checkKeywordPrefix, "f");
    CHECK_TOKEN(checkKeywordPrefix, "fo");
    CHECK_TOKEN(checkKeywordPrefix, "fu");
    CHECK_TOKEN(checkKeywordPrefix, "fun");
    CHECK_TOKEN(checkKeywordPrefix, "func");
    CHECK_TOKEN(checkKeywordPrefix, "funct");
    CHECK_TOKEN(checkKeywordPrefix, "functi");
    CHECK_TOKEN(checkKeywordPrefix, "functio");
    CHECK_TOKEN(checkKeywordPrefix, "i");
    CHECK_TOKEN(checkKeywordPrefix, "s");
    CHECK_TOKEN(checkKeywordPrefix, "se");
    CHECK_TOKEN(checkKeywordPrefix, "sel");
    CHECK_TOKEN(checkKeywordPrefix, "sele");
    CHECK_TOKEN(checkKeywordPrefix, "selec");
    CHECK_TOKEN(checkKeywordPrefix, "t");
    CHECK_TOKEN(checkKeywordPrefix, "th");
    CHECK_TOKEN(checkKeywordPrefix, "the");
    CHECK_TOKEN(checkKeywordPrefix, "u");
    CHECK_TOKEN(checkKeywordPrefix, "un");
    CHECK_TOKEN(checkKeywordPrefix, "unt");
    CHECK_TOKEN(checkKeywordPrefix, "unti");
    CHECK_TOKEN(checkKeywordPrefix, "w");
    CHECK_TOKEN(checkKeywordPrefix, "wh");
    CHECK_TOKEN(checkKeywordPrefix, "whi");
    CHECK_TOKEN(checkKeywordPrefix, "whil");

    CHECK_TOKEN(checkKeywordPrefix, "\\\nc\\\n\\\nas\\\n");
    CHECK_TOKEN(checkKeywordPrefix, "\\\nwh\\\n\\\nil\\\n");
}

void checkKeywordWithEof(const String &s) {
    CLocaleEnvironmentStub e;
    e.appendSource(String(s));
    e.setIsEof();
    CHECK(peekKeyword(e) == s);
    CHECK(e.current() == e.begin());
    CHECK(parseKeyword(e) == s);
    CHECK(e.current() == e.begin() + s.length());
}

String insertLineContinuations(const String &s) {
    String s2;
    for (auto c : s) {
        s2 += L("\\\n\\\n");
        s2 += c;
    }
    s2 += L("\\\n\\\n");
    return s2;
}

void checkExactKeyword(const String &s) {
    CHECK_FALSE(s.empty());
    checkNonKeywordWithoutEof(s + L("#"));
    checkNonKeywordWithoutEof(s + L("$"));
    checkNonKeywordWithoutEof(s + L("`"));
    checkKeywordNeedMoreSource(String(s));
    checkKeywordNeedMoreSource(insertLineContinuations(s));
    checkKeywordWithEof(String(s));

    {
        CLocaleEnvironmentStub e;
        e.appendSource(s.substr(0u, 1u));
        e.appendSource(L("\\\n\\\n"));
        e.appendSource(s.substr(1u));
        e.appendSource(L("\\\n\\\n#"));
        CHECK(parseKeyword(e) == String());
        CHECK(e.current() == e.begin());
    }
    {
        CLocaleEnvironmentStub e;
        e.appendSource(s.substr(0u, 1u));
        e.appendSource(L("\\\n\\\n"));
        e.appendSource(s.substr(1u));
        e.appendSource(L("\\\n\\\n"));
        e.setIsEof();
        CHECK(parseKeyword(e) == s);
        CHECK(e.current() == e.begin() + s.length());
    }
    {
        CLocaleEnvironmentStub e;
        e.appendSource(s.substr(0u, 1u));
        e.appendSource(L("\\\n\\\n"));
        e.appendSource(s.substr(1u));
        e.appendSource(L("\\\n\\\n;"));
        CHECK(parseKeyword(e) == s);
        CHECK(e.current() == e.begin() + s.length());
    }
}

TEST_CASE("Token, check exact keywords") {
    CHECK_TOKEN(checkExactKeyword, "!");
    CHECK_TOKEN(checkExactKeyword, "[[");
    CHECK_TOKEN(checkExactKeyword, "]]");
    CHECK_TOKEN(checkExactKeyword, "{");
    CHECK_TOKEN(checkExactKeyword, "}");
    CHECK_TOKEN(checkExactKeyword, "case");
    CHECK_TOKEN(checkExactKeyword, "do");
    CHECK_TOKEN(checkExactKeyword, "done");
    CHECK_TOKEN(checkExactKeyword, "elif");
    CHECK_TOKEN(checkExactKeyword, "else");
    CHECK_TOKEN(checkExactKeyword, "esac");
    CHECK_TOKEN(checkExactKeyword, "for");
    CHECK_TOKEN(checkExactKeyword, "function");
    CHECK_TOKEN(checkExactKeyword, "if");
    CHECK_TOKEN(checkExactKeyword, "in");
    CHECK_TOKEN(checkExactKeyword, "select");
    CHECK_TOKEN(checkExactKeyword, "then");
    CHECK_TOKEN(checkExactKeyword, "until");
    CHECK_TOKEN(checkExactKeyword, "while");
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
