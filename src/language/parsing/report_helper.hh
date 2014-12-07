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

#ifndef INCLUDED_language_parsing_report_helper_hh
#define INCLUDED_language_parsing_report_helper_hh

#include "buildconfig.h"

#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/copy.hh"
#include "language/parsing/parser.hh"
#include "language/source/stream.hh"
#include "ui/message/category.hh"
#include "ui/message/format.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

template<typename R>
class report_adder {

public:

    result<R> r;
    ui::message::category c;
    ui::message::format<> f;
    std::vector<std::shared_ptr<const ui::message::report>> rs;

    result<R> operator()(const source::stream_value &sv) {
        r.reports.emplace_back(
                c, std::move(f), common::copy(sv.first), std::move(rs));
        return std::move(r);
    }

}; // template<typename R> class report_adder

/**
 * Returns a future of (a copy of) the argument result with a new report added.
 *
 * @param r Result to be returned the returned future.
 * @param c Category of the report added to the result.
 * @param f Message of the report added to the result.
 * @param s Stream indicating the position of the report.
 * @param rs Sub-reports of the report added to the result.
 */
template<typename R>
async::future<result<R>> add_report(
        result<R> &&r,
        ui::message::category c,
        ui::message::format<> &&f,
        const source::stream &s,
        std::vector<std::shared_ptr<const ui::message::report>> &&rs = {}) {
    return s->get().map(
            report_adder<R>{std::move(r), c, std::move(f), std::move(rs)});
}

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_report_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
