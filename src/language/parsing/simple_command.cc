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
#include "simple_command.hh"

#include <functional>
#include <tuple>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/container_helper.hh"
#include "common/either.hh"
#include "common/empty.hh"
#include "common/type_tag.hh"
#include "helpermacros.h"
#include "language/parsing/joiner.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/parser.hh"
#include "language/parsing/report_helper.hh"
#include "language/parsing/token.hh"
#include "language/parsing/whitespace.hh"
#include "language/syntax/simple_command.hh"
#include "ui/message/category.hh"
#include "ui/message/format.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::async::make_future_of;
using sesh::common::empty;
using sesh::common::maybe;
using sesh::common::move;
using sesh::common::type_tag;
using sesh::language::syntax::simple_command;
using sesh::language::syntax::word;
using sesh::ui::message::category;
using sesh::ui::message::format;
using sesh::ui::message::report;

token_type_set acceptable_token_types(const simple_command &) {
    return {type_tag<word>()};
}

future<result<std::tuple<token, empty>>> parse_token_and_skip_whitespaces(
        token_type_set types, const state &s) {
    auto parser = join(
            std::bind(parse_token, types, std::placeholders::_1),
            skip_whitespaces);
    return parser(s);
}

// defined just below
future<result<simple_command>> parse_tokens(result<simple_command> &&r);

class token_acceptor {

public:

    result<simple_command> result_so_far;

    future<result<simple_command>> operator()(
            result<std::tuple<token, empty>> &&r) {
        move(r.reports, result_so_far.reports);
        if (!r.product)
            return make_future_of(std::move(result_so_far));

        result_so_far.product->state = std::move(r.product->state);

        auto &new_token = std::get<0>(r.product->value);
        switch (new_token.tag()) {
        case token::tag<word>():
            result_so_far.product->value.words.push_back(
                    std::move(new_token.value<word>()));
            return parse_tokens(std::move(result_so_far));
        }
        UNREACHABLE();
    }

}; // class token_acceptor

future<result<simple_command>> parse_tokens(result<simple_command> &&r) {
    if (!r.product)
        return make_future_of(std::move(r));

    auto types = acceptable_token_types(r.product->value);
    auto r2 = parse_token_and_skip_whitespaces(types, r.product->state);
    return std::move(r2).map(token_acceptor{std::move(r)}).unwrap();
}

future<result<simple_command_parse>> accept_nonempty_command(
        result<simple_command> &&from) {
    result<simple_command_parse> to(empty(), std::move(from.reports));
    if (auto p = from.product) {
        if (p->value.empty())
            return add_report(
                    std::move(to),
                    category::error,
                    format<>(L("empty command")),
                    p->state.rest);
        to.product.emplace_with_fallback<empty>(
                product<simple_command_parse>{
                std::move(p->value), std::move(p->state)});
    }
    return make_future_of(std::move(to));
}

}

future<result<simple_command_parse>> parse_simple_command(const state &s) {
    auto r = parse_tokens(result<simple_command>(
                product<simple_command>{simple_command(), s},
                std::vector<report>()));
    return std::move(r).map(accept_nonempty_command).unwrap();
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
