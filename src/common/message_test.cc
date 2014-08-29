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
#include "boost/format.hpp"
#include "common/message.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"

namespace {

using sesh::common::message;
using sesh::common::xchar;
using sesh::common::xstring;

TEST_CASE("Message, construction") {
    const xstring s = L("test");
    message<> m1;
    message<> m2(nullptr);
    message<> m3(L("test"));
    message<> m4(s);
    message<> m5(xstring(L("test")));
}

TEST_CASE("Message, special member functions without arguments") {
    message<> m1;
    message<> m2(m1);
    message<> m3(std::move(m2));
    m2 = m3;
    m3 = std::move(m2);
}

TEST_CASE("Message, format string") {
    message<> m1;
    CHECK(m1.format_string() == L(""));
    m1.format_string() = L("123");
    CHECK(m1.format_string() == L("123"));

    const message<> m2(L("ABC"));
    CHECK(m2.format_string() == L("ABC"));

    CHECK(message<>().format_string() == L(""));
    CHECK(message<>(xstring(L("test"))).format_string() == L("test"));
}

TEST_CASE("Message, format without arguments") {
    const message<> m(L("123"));
    message<>::format_type f(m.to_format());
    CHECK(f.str() == L("123"));

    CHECK(message<>(L"%%").to_format().str() == L("%"));

    CHECK((message<int>(L("%1%")).to_format() % 100).str() == L("100"));
}

TEST_CASE("Message, format with arguments") {
    message<int, xchar> m1(L("%2%%1%"));
    message<int, xchar>::format_type f1 = m1.to_format();
    CHECK((f1 % 1 % L('A')).str() == L("A1"));

    message<xchar> &&m2 = std::move(m1) % 2;
    message<xchar>::format_type f2 = m2.to_format();
    CHECK((f2 % L('B')).str() == L("B2"));

    message<> &&m3 = std::move(m2) % L('C');
    message<>::format_type f3 = m3.to_format();
    CHECK(f3.str() == L("C2"));
}

TEST_CASE("Message, to string") {
    CHECK(message<>(L("ABC%%")).to_string() == L("ABC%"));
    CHECK((message<int>(L("%1%%%")) % 33).to_string() == L("33%"));
    CHECK((message<xstring, xchar>(L("[%1%:%2%]")) % L("S") % L('C'))
            .to_string()
            == L("[S:C]"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
