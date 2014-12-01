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

#ifndef INCLUDED_language_parsing_option_hh
#define INCLUDED_language_parsing_option_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "async/future.hh"
#include "common/either.hh"
#include "common/empty.hh"
#include "language/parsing/parser.hh"

namespace sesh {
namespace language {
namespace parsing {

/**
 * An optional parser always succeeds by wrapping the product value of another
 * parser in a maybe.
 */
template<typename P>
class optional_parser {

public:

    using argument_type = const state &;
    using value_type = typename result_type_of<P>::type;
    using result_type = async::future<result<common::maybe<value_type>>>;

    P main_parser;

private:

    struct mapper {

        state original_state;

        product<common::maybe<value_type>> make_product(
                common::maybe<product<value_type>> &&p) {
            if (p)
                return {{std::move(p->value)}, std::move(p->state)};
            return {{}, std::move(original_state)};
        }

        result<common::maybe<value_type>> operator()(
                result<value_type> &&from) {
            return result<common::maybe<value_type>>(
                    make_product(std::move(from.product)),
                    std::move(from.reports));
        }

    }; // struct mapper

public:

    result_type operator()(argument_type s) const {
        return main_parser(s).map(mapper{s});
    }

}; // template<typename R> class optional_parser

/** Helper function to construct an optional_parser. */
template<typename P>
auto option(P &&p) -> optional_parser<typename std::decay<P>::type> {
    return {std::forward<P>(p)};
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_option_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
