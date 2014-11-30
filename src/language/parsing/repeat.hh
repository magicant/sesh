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

#ifndef INCLUDED_language_parsing_repeat_hh
#define INCLUDED_language_parsing_repeat_hh

#include "buildconfig.h"

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/container_helper.hh"
#include "common/empty.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace repeat_impl {

template<typename Parser, typename ResultList>
class repeater {

public:

    Parser parser;
    class state state;
    ResultList results;
    std::vector<ui::message::report> reports;

    template<typename T>
    async::future<result<ResultList>> operator()(product<T> &&p) {
        state = std::move(p.state);
        results.push_back(std::move(p.value));
        return (*this)();
    }

    template<typename T>
    async::future<result<ResultList>> operator()(result<T> &&r) {
        common::move(r.reports, reports);
        if (!r.product)
            return async::make_future<result<ResultList>>(
                    product<ResultList>{std::move(results), std::move(state)},
                    std::move(reports));
        return (*this)(std::move(*r.product));
    }

    async::future<result<ResultList>> operator()() {
        return parser(state).map(std::move(*this)).unwrap();
    }

}; // template<typename Parser, typename ResultList> class repeater

template<typename Parser, typename ResultList>
class nonempty_repeater {

public:

    using repeater_type = class repeater<Parser, ResultList>;

    repeater_type repeater;

    template<typename T>
    async::future<result<ResultList>> operator()(result<T> &&r) {
        if (!r.product)
            return async::make_future<result<ResultList>>(
                    common::empty(), std::move(r.reports));
        common::move(r.reports, repeater.reports);
        return repeater(std::move(*r.product));
    }

}; // template<typename Parser, typename ResultList> class nonempty_repeater

} // namespace repeat_impl

/**
 * Applies the argument parser function repeatedly until it fails. Results are
 * accumulated in a vector (or a compatible container). The repeat as a whole
 * always succeeds regardless of how many times the argument parser succeeds.
 *
 * Reports from the repeated parsing (including the ones from the last failed
 * parsing) are accumulated and returned in the final result.
 *
 * The parser function should not be a nop; otherwise the repeat would never
 * end.
 *
 * @tparam P Type of the parser function.
 * @tparam ResultList Type of the container which collects the results of
 * repeated parsing. It must be move-constructible, destructible, and have the
 * <code>push_back(typename result_type_of&lt;P>::type &&)</code> method.
 * @param p Parser function. Must be convertible to
 * <code>std::function&lt;parser&lt;T>></code> for some @c T.
 * @param s State in which the repeat starts.
 * @param results Container to which the results are added.
 */
template<
        typename P,
        typename ResultList = std::vector<typename result_type_of<P>::type>>
async::future<result<ResultList>> repeat(
        P &&p, const state &s, ResultList &&results = ResultList()) {
    repeat_impl::repeater<typename std::decay<P>::type, ResultList> r{
            std::forward<P>(p), s, std::forward<ResultList>(results), {}};
    return r();
}

/**
 * Like {@link repeat}, but requires the argument parser to succeed at least
 * once. If it fails in the first round, one_or_more also fails with the same
 * reports.
 */
template<
        typename P,
        typename ResultList = std::vector<typename result_type_of<P>::type>>
async::future<result<ResultList>> one_or_more(
        P &&p, const state &s, ResultList &&results = ResultList()) {
    using Parser = typename std::decay<P>::type;
    repeat_impl::nonempty_repeater<Parser, ResultList> r{{
            std::forward<P>(p), s, std::forward<ResultList>(results), {}}};
    return r.repeater.parser(s).map(std::move(r)).unwrap();
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_repeat_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
