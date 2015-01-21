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
#include "sequence.hh"

#include <utility>
#include "async/future.hh"
#include "language/parsing/and_or_list.hh"
#include "language/parsing/mapper.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/sequence.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::language::syntax::and_or_list;
using sesh::language::syntax::sequence;

class converter {

public:

    sequence operator()(and_or_list &&aol) const {
        sequence s;
        s.and_or_lists.push_back(std::move(aol));
        return s;
    }

}; // class converter

} // namespace

future<result<sequence_parse>> parse_sequence(const state &s) {
    return map_value(
            parse_and_or_list(s),
            [](and_or_list_parse &&ap) -> sequence_parse {
                return std::move(ap).map(converter());
            });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
