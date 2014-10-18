/* Copyright (C) 2013 WATANABE Yuki
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

#include <memory>
#include <stdexcept>
#include "catch.hpp"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/command.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline.hh"

namespace {

using sesh::language::syntax::and_or_list;
using sesh::language::syntax::command;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::pipeline;

pipeline pipeline_stub() {
    pipeline p(pipeline::exit_status_mode_type::negated);
    p.commands().push_back(pipeline::command_pointer(new command()));
    return p;
}

TEST_CASE("And-or list constructor 1") {
    and_or_list aol(pipeline_stub());
    aol.rest().emplace_back(conditional_pipeline::condition_type::and_then);
    aol.rest()[0].pipeline().commands().push_back(
            pipeline::command_pointer(new command()));
    aol.rest()[0].pipeline().commands().push_back(
            pipeline::command_pointer(new command()));

    CHECK(aol.synchronicity() == and_or_list::synchronicity_type::sequential);
    CHECK(aol.first().exit_status_mode() ==
            pipeline::exit_status_mode_type::negated);
    CHECK(aol.first().commands().size() == 1);
    CHECK(aol.rest().size() == 1);

    const pipeline &p1 = aol.rest()[0].pipeline();
    CHECK(p1.commands().size() == 2);
}

TEST_CASE("And-or list constructor 2") {
    and_or_list aol(
            pipeline(pipeline::exit_status_mode_type::negated),
            and_or_list::synchronicity_type::asynchronous);

    CHECK(aol.first().commands().empty());
    CHECK(aol.first().exit_status_mode() ==
            pipeline::exit_status_mode_type::negated);
    CHECK(aol.rest().empty());
    CHECK(aol.synchronicity() ==
            and_or_list::synchronicity_type::asynchronous);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
