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

#ifndef INCLUDED_ui_message_report_test_helper_hh
#define INCLUDED_ui_message_report_test_helper_hh

#include "buildconfig.h"

#include <algorithm>
#include "catch.hpp"
#include "ui/message/report.hh"

namespace sesh {
namespace ui {
namespace message {

void check_equal(const report &l, const report &r) {
    CHECK(l.category == r.category);
    CHECK(l.text.to_string() == r.text.to_string());
    CHECK(l.position.head == r.position.head);
    CHECK(l.position.index == r.position.index);
    CHECK(l.subreports.size() == r.subreports.size());
    for (decltype(l.subreports)::size_type i = 0;
            i < std::min(l.subreports.size(), r.subreports.size());
            ++i) {
        REQUIRE(l.subreports[i] != nullptr);
        REQUIRE(r.subreports[i] != nullptr);
        check_equal(*l.subreports[i], *r.subreports[i]);
    }
}

} // namespace message
} // namespace ui
} // namespace sesh

#endif // #ifndef INCLUDED_ui_message_report_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
