/* Copyright (C) 2015 WATANABE Yuki
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
#include "and_or_list.hh"

#include <utility>
#include "async/future.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/pipeline.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/pipeline.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::language::syntax::and_or_list;
using sesh::language::syntax::pipeline;

class converter {

public:

    and_or_list operator()(pipeline &&p) const {
        and_or_list aol;
        aol.first = std::move(p);
        return aol;
    }

}; // class converter

} // namespace

future<result<and_or_list_parse>> parse_and_or_list(const state &s) {
    return map_value(
            parse_pipeline(s),
            [](pipeline_parse &&pp) -> and_or_list_parse {
                return std::move(pp).map(converter());
            });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
