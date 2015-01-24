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
#include "pipeline.hh"

#include <utility>
#include "async/future.hh"
#include "language/parsing/command.hh"
#include "language/parsing/mapper.hh"

namespace sesh {
namespace language {
namespace parsing {

namespace {

using sesh::async::future;
using sesh::language::syntax::pipeline;

class converter {

public:

    pipeline operator()(command_pointer &&cp) const {
        pipeline p(pipeline::exit_status_mode_type::straight);
        p.commands.push_back(std::move(cp));
        return p;
    }

}; // class converter

} // namespace

future<result<pipeline_parse>> parse_pipeline(const state &s) {
    return map_value(
            parse_command(s),
            [](command_parse &&cp) -> pipeline_parse {
                return std::move(cp).map(converter());
            });
}

} // namespace parsing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
