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

#ifndef INCLUDED_language_parsing_mapper_hh
#define INCLUDED_language_parsing_mapper_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "async/future.hh"
#include "common/copy.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace mapper_impl {

template<typename F>
class product_mapper {

public:

    F f;

    template<
            typename From,
            typename To = typename result_type_of<
                    typename std::result_of<F(product<From> &&)>::type>::type>
    result<To> operator()(result<From> &&from) {
        result<To> to(common::empty(), std::move(from.reports));
        if (from.product)
            to.product.template emplace_with_fallback<common::empty>(
                    f(std::move(*from.product)));
        return to;
    }

}; // template<typename F> class product_mapper

template<typename F>
class value_mapper {

public:

    F f;

    template<
            typename From,
            typename To = typename std::result_of<F(From &&)>::type>
    product<To> operator()(product<From> &&from) {
        return {f(std::move(from.value)), std::move(from.state)};
    }

}; // template<typename F> class value_mapper

} // namespace mapper_impl

template<
        typename From,
        typename Mapper,
        typename To = typename result_type_of<
                typename std::result_of<Mapper(product<From>)>::type>::type>
auto map_product(async::future<result<From>> &&f, Mapper &&m)
        -> async::future<result<To>> {
    using M = mapper_impl::product_mapper<typename std::decay<Mapper>::type>;
    return std::move(f).map(M{std::forward<Mapper>(m)});
}

template<
        typename From,
        typename Mapper,
        typename To = typename std::result_of<Mapper(From)>::type>
auto map_value(async::future<result<From>> &&f, Mapper &&m)
        -> async::future<result<To>> {
    using M = mapper_impl::value_mapper<typename std::decay<Mapper>::type>;
    return map_product(std::move(f), M{std::forward<Mapper>(m)});
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_mapper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
