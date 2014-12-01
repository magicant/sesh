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

#ifndef INCLUDED_language_parsing_joiner_hh
#define INCLUDED_language_parsing_joiner_hh

#include "buildconfig.h"

#include <tuple>
#include <type_traits>
#include <vector>
#include "async/future.hh"
#include "common/container_helper.hh"
#include "common/empty.hh"
#include "common/integer_sequence.hh"
#include "language/parsing/constant_parser.hh"
#include "language/parsing/parser.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * A parser joiner applies some parsers in a row and returns a tuple of all the
 * results if all the parsers succeed.
 *
 * @tparam Tuple Type of a tuple to which the results of the remaining parsers
 * are appended.
 * @tparam Parser Types of parsers that will be applied one by one.
 */
template<typename Tuple, typename... Parser>
class joiner;

template<typename... R>
class joiner<std::tuple<R...>> : public constant_parser<std::tuple<R...>> {

public:

    using input_tuple_type = std::tuple<R...>;
    using result_tuple_type = input_tuple_type;

    joiner(input_tuple_type &&t, std::vector<ui::message::report> &&r) :
            constant_parser<std::tuple<R...>>{std::move(t), std::move(r)} { }

}; // class joiner<>

template<typename... R, typename FirstParser, typename... OtherParser>
class joiner<std::tuple<R...>, FirstParser, OtherParser...> {

private:

    using first_parser_result_type =
            typename result_type_of<FirstParser>::type;
    using next_joiner_type = joiner<
            std::tuple<R..., first_parser_result_type>, OtherParser...>;

public:

    using input_tuple_type = std::tuple<R...>;
    using result_tuple_type = typename next_joiner_type::result_tuple_type;
    using argument_type = const state &;
    using result_type = typename next_joiner_type::result_type;

private:

    struct mapper {

        input_tuple_type results;
        std::vector<ui::message::report> reports;
        std::tuple<OtherParser...> other_parsers;

        template<typename T, std::size_t... I>
        result_type operator()(product<T> &&p, common::index_sequence<I...>) {
            next_joiner_type j(
                    std::tuple_cat(
                        std::move(results),
                        std::forward_as_tuple(std::move(p.value))),
                    std::move(reports),
                    std::move(std::get<I>(other_parsers))...);
            return j(p.state);
        }

        template<typename T>
        result_type operator()(result<T> &&t) {
            common::move(t.reports, reports);
            if (!t.product)
                return async::make_future<result<result_tuple_type>>(
                        common::empty(), std::move(reports));
            return (*this)(
                    std::move(*t.product),
                    common::index_sequence_for<OtherParser...>());
        }

    }; // struct mapper

    FirstParser m_first_parser;
    mapper m_mapper;

public:

    explicit joiner(
            input_tuple_type &&t,
            std::vector<ui::message::report> &&r,
            const FirstParser &fp,
            const OtherParser &... op) :
            m_first_parser(fp),
            m_mapper{
                    std::move(t),
                    std::move(r),
                    std::tuple<OtherParser...>{op...}} { }

    result_type operator()(const state &s) const {
        return m_first_parser(s).map(m_mapper).unwrap();
    }

}; // class joiner<FirstParser, OtherParser...>

/** Returns a joiner of the argument parsers. */
template<typename... Parser>
auto join(Parser &&... p)
        -> joiner<std::tuple<>, typename std::decay<Parser>::type...> {
    return joiner<std::tuple<>, typename std::decay<Parser>::type...>(
            std::tuple<>{},
            std::vector<ui::message::report>{},
            std::forward<Parser>(p)...);
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_joiner_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
