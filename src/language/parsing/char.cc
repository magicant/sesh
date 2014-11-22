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
#include "char.hh"

#include <functional>
#include <utility>
#include "async/future.hh"
#include "common/constant_function.hh"
#include "common/copy.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/source/stream.hh"

namespace {

using sesh::async::future;
using sesh::common::constant;
using sesh::common::copy;
using sesh::common::xchar;
using sesh::common::xchar_traits;
using sesh::language::source::stream_value;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

namespace {

class char_tester {

public:

    std::function<bool(xchar)> predicate;
    class context context;

    result<xchar> operator()(const stream_value &sv) {
        if (sv.first == nullptr || !predicate(*sv.first))
            return {};
        return product<xchar>{*sv.first, {sv.second, std::move(context)}};
    }

}; // class char_tester

} // namespace

auto test_char(const std::function<bool(xchar)> &p, const state &s)
        -> future<result<xchar>> {
    return s.rest->get().map(char_tester{p, s.context});
}

future<result<xchar>> parse_char(xchar c, const state &s) {
    using namespace std::placeholders;
    return test_char(std::bind(xchar_traits::eq, c, _1), s);
}

future<result<xchar>> accept_char(const state &s) {
    return test_char(constant(true), s);
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
