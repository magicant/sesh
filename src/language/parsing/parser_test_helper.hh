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

#ifndef INCLUDED_language_parsing_parser_test_helper_hh
#define INCLUDED_language_parsing_parser_test_helper_hh

#include "buildconfig.h"

#include <iterator>
#include <memory>
#include <utility>
#include "catch.hpp"
#include "common/either.hh"
#include "common/nop.hh"
#include "common/xstring.hh"
#include "language/parsing/parser.hh"
#include "language/source/fragment.hh"
#include "language/source/fragment_test_helper.hh"
#include "language/source/stream.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

inline void check_equal(const context &l, const context &r) {
    CHECK(l.dummy == r.dummy);
}

inline source::stream stream_stub(
        const source::fragment::value_type &head = {},
        const source::stream &tail = source::empty_stream()) {
    if (head.empty())
        return tail;
    return stream_of(
            source::fragment_position(
                    std::make_shared<source::fragment>(head)),
            tail);
}

inline context default_context_stub() {
    return {0};
}

template<typename P, typename C>
void check_parser(P &&parse, const state &s, C &&check_result) {
    using R = typename result_type_of<P>::type;
    auto r = std::forward<P>(parse)(s);

    bool called = false;
    std::move(r).then(
            [&called, &check_result](const common::trial<result<R>> &t) {
        REQUIRE(t);
        std::forward<C>(check_result)(*t);
        called = true;
    });
    CHECK(called);
}

/**
 * Checks if the argument parser function successfully parses the argument
 * source code string.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * succeeds.
 * @param c Context in which parsing starts.
 * @param check_result Function that checks if the result is correct. Must be
 * callable with <code>const R &amp;</code> where the parser returns
 * <code>future&lt;result&lt;R>></code>.
 */
template<typename P, typename C>
void check_parser_success_result(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c,
        C &&check_result) {
    using R = typename result_type_of<P>::type;
    check_parser(
            std::forward<P>(parse),
            {stream_stub(src), c},
            [&check_result](const result<R> &r) {
                REQUIRE(r.product);
                std::forward<C>(check_result)(r.product->value);
            });
}

/**
 * Checks if the argument parser function successfully parses the argument
 * source code string.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * succeeds.
 * @param check_result Function that checks if the result is correct. Must be
 * callable with <code>const R &amp;</code> where the parser returns
 * <code>future&lt;result&lt;R>></code>.
 */
template<typename P, typename C>
void check_parser_success_result(
        P &&parse, const source::fragment::value_type &src, C &&check_result) {
    check_parser_success_result(
            std::forward<P>(parse),
            src,
            default_context_stub(),
            std::forward<C>(check_result));
}

/**
 * Checks if the argument parser function successfully parses the argument
 * source code string.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * succeeds.
 * @param c Context in which parsing starts.
 * @param check_context Function that checks if the resultant context is
 * correct. Must be callable with <code>const context &amp;</code>.
 */
template<typename P, typename C>
void check_parser_success_context(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c,
        C &&check_context) {
    using R = typename result_type_of<P>::type;
    check_parser(
            std::forward<P>(parse),
            {stream_stub(src), c},
            [&check_context](const result<R> &r) {
                REQUIRE(r.product);
                std::forward<C>(check_context)(r.product->state.context);
            });
}

/**
 * Checks if the argument parser function successfully parses the argument
 * source code string. Also checks if the parser is context-free i.e. the
 * parser does not alter the context.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * succeeds.
 */
template<typename P>
void check_parser_success_context_free(
        P &&parse, const source::fragment::value_type &src) {
    const context c = {4567};
    check_parser_success_context(
            std::forward<P>(parse),
            src,
            c,
            [&c](const context &c2) { check_equal(c2, c); });
}

/**
 * Checks if the argument parser function successfully parses the argument
 * source code string and returns a state with an expected fragment position.
 *
 * @param parse Parser function
 * @param source_to_parse Source code fragment string that is expected to be
 * parsed (consumed) by the parser.
 * @param trailer Source code fragment string that follows @c source_to_parse.
 * The parser may look ahead over the trailer but is not expected to consume
 * it.
 */
template<typename P>
void check_parser_success_rest(
        P &&parse,
        const source::fragment::value_type &source_to_parse,
        const source::fragment::value_type &trailer = {},
        const context &c = default_context_stub()) {
    source::fragment_position fp;
    if (!source_to_parse.empty() || !trailer.empty())
        fp.head = std::make_shared<source::fragment>(
                source_to_parse + trailer);

    auto s = source::empty_stream();
    if (fp != nullptr)
        s = stream_of(fp, s);

    using R = typename result_type_of<P>::type;
    check_parser(
            std::forward<P>(parse),
            {s, c},
            [&](const result<R> &r) {
                REQUIRE(r.product);

                using T = common::trial<source::stream_value>;
                const source::stream &s2 = r.product->state.rest;
                bool called = false;
                s2->get().then([&](const T &t) {
                    REQUIRE(t);
                    CHECK(t->first == std::next(fp, source_to_parse.length()));
                    called = true;
                });
                CHECK(called);
            });
}

/**
 * Checks if the argument parser function does not read more than the argument
 * source code string while parsing it.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * succeeds. The check fails if the parser tries to read more than the source.
 * @param c Context in which parsing starts.
 */
template<typename P>
void check_parser_no_excess(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c = default_context_stub()) {
    auto failing_stream = source::stream([]() -> source::stream_value_future {
        FAIL("The parser is reading too much source code");
        return source::empty_stream_value_future();
    });
    check_parser(
            std::forward<P>(parse),
            {stream_stub(src, std::move(failing_stream)), c},
            common::nop());
}

/**
 * Checks if the argument parser function fails to parse the argument source
 * code string.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read before it
 * fails. The check will fail if the parser tries to read more than the source.
 * @param c Context in which parsing starts.
 */
template<typename P>
void check_parser_failure(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c = default_context_stub()) {
    using R = typename result_type_of<P>::type;
    check_parser(
            std::forward<P>(parse),
            {stream_stub(src), c},
            [](const result<R> &r) { CHECK_FALSE(r.product); });
}

/**
 * Checks the reports returned by the argument parser function.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read. The
 * check will fail if the parser tries to read more than the source.
 * @param c Context in which parsing starts.
 * @param check_reports Function that checks if the reports are correct. Must
 * be callable with <code>const std::vector&lt;report> &amp;</code>.
 */
template<typename P, typename C>
void check_parser_reports(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c,
        C &&check_reports) {
    using R = typename result_type_of<P>::type;
    check_parser(
            std::forward<P>(parse),
            {stream_stub(src), c},
            [&check_reports](const result<R> &r) {
                std::forward<C>(check_reports)(r.reports);
            });
}

/**
 * Checks if the argument parser function yields no reports when applied to the
 * argument source code.
 *
 * @param parse Parser function
 * @param src Source code fragment string that the parser may read. The
 * check will fail if the parser tries to read more than the source.
 * @param c Context in which parsing starts.
 */
template<typename P>
void check_parser_no_reports(
        P &&parse,
        const source::fragment::value_type &src,
        const context &c = default_context_stub()) {
    check_parser_reports(
            std::forward<P>(parse),
            src,
            c,
            [](const std::vector<ui::message::report> &r) {
                CHECK(r.empty());
            });
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_parser_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
