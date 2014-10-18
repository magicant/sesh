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
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::command;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::pipeline;

void add_command(pipeline &p) {
    p.commands().push_back(pipeline::command_pointer(new command()));
}

TEST_CASE("Conditional pipeline constructors") {
    conditional_pipeline cp1(conditional_pipeline::condition_type::and_then);
    add_command(cp1.pipeline());
    CHECK(cp1.condition() == conditional_pipeline::condition_type::and_then);
    CHECK(cp1.pipeline().commands().size() == 1);

    conditional_pipeline cp2(conditional_pipeline::condition_type::or_else);
    add_command(cp2.pipeline());
    add_command(cp2.pipeline());
    add_command(cp2.pipeline());
    CHECK(cp2.condition() == conditional_pipeline::condition_type::or_else);
    CHECK(cp2.pipeline().commands().size() == 3);

    conditional_pipeline cp3(
            conditional_pipeline::condition_type::and_then,
            nullptr);
    add_command(cp3.pipeline());
    CHECK(cp3.condition() == conditional_pipeline::condition_type::and_then);
    CHECK(cp3.pipeline().commands().size() == 1);

    conditional_pipeline cp4(
            conditional_pipeline::condition_type::or_else,
            nullptr);
    add_command(cp4.pipeline());
    add_command(cp4.pipeline());
    CHECK(cp4.condition() == conditional_pipeline::condition_type::or_else);
    CHECK(cp4.pipeline().commands().size() == 2);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
