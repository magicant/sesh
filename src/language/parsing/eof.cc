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
#include "eof.hh"

#include <utility>
#include "async/future.hh"
#include "language/parsing/parser.hh"
#include "language/source/stream.hh"

namespace {

using sesh::async::future;
using sesh::language::parsing::product;
using sesh::language::parsing::result;
using sesh::language::parsing::state;
using sesh::language::source::stream_value;

} // namespace

namespace sesh {
namespace language {
namespace parsing {

future<result<eof>> parse_eof(const state &s) {
    auto &c = s.context;
    return s.rest->get().map([c](const stream_value &sv) -> result<eof> {
        if (sv.first != nullptr)
            return {};
        return product<eof>{eof(), {sv.second, c}};
    });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
