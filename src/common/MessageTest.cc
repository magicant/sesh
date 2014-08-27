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
#include "common/Message.hh"
#include "common/String.hh"
#include "common/xchar.hh"

namespace {

using sesh::common::Message;
using sesh::common::String;
using sesh::common::xchar;

TEST_CASE("Message, construction") {
    const String s = L("test");
    Message<> m1;
    Message<> m2(nullptr);
    Message<> m3(L("test"));
    Message<> m4(s);
    Message<> m5(String(L("test")));
}

TEST_CASE("Message, special member functions without arguments") {
    Message<> m1;
    Message<> m2(m1);
    Message<> m3(std::move(m2));
    m2 = m3;
    m3 = std::move(m2);
}

TEST_CASE("Message, format string") {
    Message<> m1;
    CHECK(m1.formatString() == L(""));
    m1.formatString() = L("123");
    CHECK(m1.formatString() == L("123"));

    const Message<> m2(L("ABC"));
    CHECK(m2.formatString() == L("ABC"));

    CHECK(Message<>().formatString() == L(""));
    CHECK(Message<>(String(L("test"))).formatString() == L("test"));
}

TEST_CASE("Message, format without arguments") {
    const Message<> m(L("123"));
    Message<>::Format f(m.toFormat());
    CHECK(f.str() == L("123"));

    CHECK(Message<>(L"%%").toFormat().str() == L("%"));

    CHECK((Message<int>(L("%1%")).toFormat() % 100).str() == L("100"));
}

TEST_CASE("Message, format with arguments") {
    Message<int, xchar> m1(L("%2%%1%"));
    Message<int, xchar>::Format f1 = m1.toFormat();
    CHECK((f1 % 1 % L('A')).str() == L("A1"));

    Message<xchar> &&m2 = std::move(m1) % 2;
    Message<xchar>::Format f2 = m2.toFormat();
    CHECK((f2 % L('B')).str() == L("B2"));

    Message<> &&m3 = std::move(m2) % L('C');
    Message<>::Format f3 = m3.toFormat();
    CHECK(f3.str() == L("C2"));
}

TEST_CASE("Message, to string") {
    CHECK(Message<>(L("ABC%%")).toString() == L("ABC%"));
    CHECK((Message<int>(L("%1%%%")) % 33).toString() == L("33%"));
    CHECK((Message<String, xchar>(L("[%1%:%2%]")) % L("S") % L('C')).toString()
            == L("[S:C]"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
