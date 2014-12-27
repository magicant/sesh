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
#include "command.hh"

#include <utility>
#include "async/future.hh"
#include "common/direct_initialize.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"
#include "language/parsing/mapper.hh"
#include "language/parsing/simple_command.hh"
#include "language/syntax/command.hh"
#include "language/syntax/simple_command.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::common::direct_initialize;
using sesh::common::make_shared_visitable;
using sesh::common::type_tag;
using sesh::language::syntax::command;
using sesh::language::syntax::simple_command;

class converter {

public:

    command_pointer operator()(simple_command &&sc) const {
        return make_shared_visitable<command>(std::move(sc));
    }

}; // class converter

} // namespace

future<result<command_parse>> parse_command(const state &s) {
    return map_value(
            parse_simple_command(s),
            [](simple_command_parse &&scp) -> command_parse {
                return std::move(scp).map(converter());
            });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
