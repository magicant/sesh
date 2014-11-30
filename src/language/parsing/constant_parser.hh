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

#ifndef INCLUDED_language_parsing_constant_parser_hh
#define INCLUDED_language_parsing_constant_parser_hh

#include "buildconfig.h"

#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/copy.hh"
#include "language/parsing/parser.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

/** A parser that always succeeds with the same result. */
template<typename R>
class constant_parser {

public:

    using argument_type = const state &;
    using value_type = R;
    using result_type = async::future<result<value_type>>;

    value_type value;
    std::vector<ui::message::report> reports;

    result_type operator()(argument_type s) const {
        return async::make_future<result<value_type>>(
                product<value_type>{value, s}, common::copy(reports));
    }

}; // template<typename R> class constant_parser

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_constant_parser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
