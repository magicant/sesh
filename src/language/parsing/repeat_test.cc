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

#include <functional>
#include <locale>
#include "async/future.hh"
#include "catch.hpp"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/parsing/char.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/parser_test_helper.hh"
#include "language/parsing/repeat.hh"
#include "ui/message/category.hh"
#include "ui/message/report.hh"
#include "ui/message/report_test_helper.hh"

namespace {

using sesh::async::future;
using sesh::async::make_future_of;
using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::parsing::check_parser_failure;
using sesh::language::parsing::check_parser_no_reports;
using sesh::language::parsing::check_parser_reports;
using sesh::language::parsing::check_parser_success_context;
using sesh::language::parsing::check_parser_success_rest;
using sesh::language::parsing::check_parser_success_result;
using sesh::language::parsing::context;
using sesh::language::parsing::one_or_more;
using sesh::language::parsing::product;
using sesh::language::parsing::repeat;
using sesh::language::parsing::result;
using sesh::language::parsing::state;
using sesh::language::parsing::test_char;
using sesh::ui::message::category;
using sesh::ui::message::report;

constexpr bool is_a_or_b(xchar c) noexcept {
    return c == L('a') || c == L('b');
}

TEST_CASE("repeat passes argument container if parser fails in first round") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return repeat(
                        std::bind(test_char, is_a_or_b, _1), s, {L('#')});
            },
            {},
            [](const std::vector<xchar> &v) {
                CHECK(v == std::vector<xchar>{L('#')});
            });
}

future<result<int>> modify_context_and_report(const state &s) {
    if (s.context.dummy >= 2) {
        result<int> r;
        r.reports.emplace_back(category::error);
        return make_future_of(std::move(r));
    }

    result<int> r(product<int>{0, s});
    ++r.product->state.context.dummy;
    r.reports.emplace_back(category::warning);
    return make_future_of(std::move(r));
}

TEST_CASE("repeat passes context") {
    check_parser_success_context(
            [](const state &s) {
                return repeat(modify_context_and_report, s);
            },
            {},
            {std::locale(), 0},
            [](const context &c) {
                CHECK(c.locale == std::locale());
                CHECK(c.dummy == 2);
            });
}

TEST_CASE("repeat: 1 result (with EOF)") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return repeat(
                        std::bind(test_char, is_a_or_b, _1),
                        s,
                        xstring(L("!")));
            },
            L("a"),
            [](const xstring &s) { CHECK(s == L("!a")); });
}

TEST_CASE("repeat: 1 result (with unmatched trailer)") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return repeat(
                        std::bind(test_char, is_a_or_b, _1),
                        s,
                        xstring(L("!")));
            },
            L("ax"),
            [](const xstring &s) { CHECK(s == L("!a")); });
}

TEST_CASE("repeat: 3 results") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return repeat(
                        std::bind(test_char, is_a_or_b, _1), s, xstring());
            },
            L("aba"),
            [](const xstring &s) { CHECK(s == L("aba")); });
}

TEST_CASE("repeat: rest of 3 results") {
    using namespace std::placeholders;
    check_parser_success_rest(
            [](const state &s) {
                return repeat(std::bind(test_char, is_a_or_b, _1), s);
            },
            L("aba"),
            L("x"));
}

TEST_CASE("repeat accumulates reports from successful parses") {
    check_parser_reports(
            [](const state &s) {
                return repeat(modify_context_and_report, s);
            },
            {},
            [](const std::vector<report> &r) {
                REQUIRE(r.size() == 2);
                check_equal(r[0], {category::warning});
                check_equal(r[1], {category::warning});
            });
}

future<result<int>> failer(const state &) {
    result<int> r;
    r.reports.emplace_back(category::error);
    return make_future_of(std::move(r));
}

TEST_CASE("one_or_more fails if parser fails first") {
    check_parser_failure(
            [](const state &s) { return one_or_more(failer, s); }, {});
}

TEST_CASE("Failed one_or_more returns reports from parser") {
    check_parser_reports(
            [](const state &s) { return one_or_more(failer, s); },
            {},
            [](const std::vector<report> &r) {
                REQUIRE(r.size() == 1);
                check_equal(r[0], {category::error});
            });
}

TEST_CASE("one_or_more: 1 result") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return one_or_more(
                        std::bind(test_char, is_a_or_b, _1),
                        s,
                        xstring(L("!")));
            },
            L("a"),
            [](const xstring &s) { CHECK(s == L("!a")); });
}

TEST_CASE("one_or_more: 3 results") {
    using namespace std::placeholders;
    check_parser_success_result(
            [](const state &s) {
                return one_or_more(
                        std::bind(test_char, is_a_or_b, _1), s, xstring());
            },
            L("aba"),
            [](const xstring &s) { CHECK(s == L("aba")); });
}

TEST_CASE("one_or_more: rest of 3 results") {
    using namespace std::placeholders;
    check_parser_success_rest(
            [](const state &s) {
                return one_or_more(std::bind(test_char, is_a_or_b, _1), s);
            },
            L("aba"),
            L("x"));
}

TEST_CASE("one_or_more accumulates reports from successful parses") {
    check_parser_reports(
            [](const state &s) {
                return one_or_more(modify_context_and_report, s);
            },
            {},
            [](const std::vector<report> &r) {
                REQUIRE(r.size() == 2);
                check_equal(r[0], {category::warning});
                check_equal(r[1], {category::warning});
            });
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
