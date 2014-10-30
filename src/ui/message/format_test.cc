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

#include <utility>
#include "boost/format.hpp"
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "ui/message/format.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::ui::message::format;

TEST_CASE("Format construction") {
    const xstring s = L("test");
    format<> f1;
    format<> f2(nullptr);
    format<> f3(L("test"));
    format<> f4(s);
    format<> f5(xstring(L("test")));
}

TEST_CASE("Format special member functions") {
    format<> f1;
    format<> f2(f1);
    format<> f3(std::move(f2));
    f2 = f3;
    f3 = std::move(f2);
}

TEST_CASE("Format: format_string") {
    format<> f1;
    CHECK(f1.format_string() == L(""));
    f1.format_string() = L("123");
    CHECK(f1.format_string() == L("123"));

    const format<> f2(L("ABC"));
    CHECK(f2.format_string() == L("ABC"));

    CHECK(format<>().format_string().empty());
    CHECK(format<>(xstring(L("test"))).format_string() == L("test"));
}

TEST_CASE("Format: to_string") {
    CHECK(format<>(L("ABC%%")).to_string() == L("ABC%"));
    CHECK((format<int>(L("%1%%%")) % 33).to_string() == L("33%"));
    CHECK((format<xstring, xchar>(L("[%1%:%2%]")) % L("S") % L('C'))
            .to_string()
            == L("[S:C]"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
