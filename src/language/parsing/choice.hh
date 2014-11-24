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

#ifndef INCLUDED_language_parsing_choice_hh
#define INCLUDED_language_parsing_choice_hh

#include "buildconfig.h"

#include <tuple>
#include <type_traits>
#include <utility>
#include "common/integer_sequence.hh"
#include "common/logic_helper.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * A choice is a combination of several parsers. The choice applies the parsers
 * in order until any of them succeeds. It returns the first successful result.
 * If none of the parsers succeeds, it returns the last failed result.
 *
 * @tparam P0 Type of the first parser.
 * @tparam P1 Type of the other parsers.
 */
template<typename P0, typename... P1>
class choice;

template<typename P>
class choice<P> : public P {

    using P::P;

}; // class choice<P>

template<typename P0, typename P1, typename... P2>
class choice<P0, P1, P2...> : private std::tuple<P0, P1, P2...> {

private:

    using first_parser_type = P0;
    using second_parser_type = choice<P1, P2...>;

public:

    using argument_type = const state &;
    using result_type = typename common::same_type<
            typename result_of<first_parser_type(argument_type)>::type,
            typename result_of<second_parser_type(argument_type)>::type>::type;

    template<typename... P>
    choice(P &&... p) : std::tuple<P0, P1, P2...>(std::forward<P>(p)...) { }

private:

    struct mapper {

        second_parser_type remainders;
        class state state;

        async::future<result<result_type>> operator()(
                result<result_type> &&r) {
            if (r.product)
                return async::make_future_of(std::move(r));
            return remainders(state);
        }

    }; // class mapper

    template<std::size_t I, std::size_t... J>
    mapper make_mapper(const state &s, common::index_sequence<I, J...>) const {
        return {{std::get<J>(*this)...}, s};
    }

public:

    async::future<result<result_type>> operator()(argument_type s) const {
        auto &p0 = std::get<0>(*this);
        auto is = common::index_sequence_for<P0, P1, P2...>();
        return p0(s).map(make_mapper(s, is)).unwrap();
    }

}; // class choice<P0, P1, P2...>

template<typename... P>
choice<typename std::decay<P>::type...> choose(P &&... p) {
    return choice<typename std::decay<P>::type...>(std::forward<P>(p)...);
}



/**
 * A choice is a combination of two parsers. The choice first applies the first
 * parser. If it succeeds, its result will be the result of the choice.
 * Otherwise, the second parser is applied and its result is returned.
 */
#if 0
TODO
template<typename P0, typename P1>
class choice : private std::tuple<P0, P1> {

public:

    using argument_type = const state &;
    using result_type = typename common::same_type<
            typename result_type_of<P0>::type,
            typename result_type_of<P1>::type>::type;

    using std::tuple<P0, P1>::tuple;

    async::future<result<result_type>> operator()(argument_type s) const {
        auto &p0 = std::get<0>(*this);
        auto &p1 = std::get<1>(*this);
        return p0(s).map([s, p1](result<result_type> &&r) {
            if (r.product)
                return async::make_future_of(std::move(r));
            return p1(s);
        }).unwrap();
    }

}; // template<typename T> class choice

/** Creates a choice from the argument parsers. */
template<typename P0, typename P1>
auto choose(P0 &&p0, P1 &&p1)
        -> choice<typename std::decay<P0>::type, typename std::decay<P1>::type>
{
    choice<typename std::decay<P0>::type, typename std::decay<P1>::type> c(
            std::move(p0), std::move(p1));
    return c;
}
#endif

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_choice_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
