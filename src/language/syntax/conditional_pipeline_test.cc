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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <memory>
#include <stdexcept>
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/printer.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::command;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::pipeline;
using sesh::language::syntax::printer;

class command_stub : public command {
private:
    xstring mString;
public:
    explicit command_stub(const xchar *s) : command(), mString(s) { }
    void print(printer &) const override;
};

void command_stub::print(printer &p) const {
    p << mString;
    p.delayed_characters() << L(' ');
}

void add_command(pipeline &p, const xchar *s) {
    p.commands().push_back(pipeline::command_pointer(new command_stub(s)));
}

std::unique_ptr<pipeline> new_pipeline(const xchar *s) {
    std::unique_ptr<pipeline> p(
            new pipeline(pipeline::exit_status_mode_type::negated));
    add_command(*p, s);
    return p;
}

TEST_CASE("Conditional pipeline constructors") {
    conditional_pipeline cp1(conditional_pipeline::condition_type::and_then);
    add_command(cp1.pipeline(), L("pipeline 1"));
    CHECK(cp1.condition() == conditional_pipeline::condition_type::and_then);
    CHECK(cp1.pipeline().commands().size() == 1);

    conditional_pipeline cp2(conditional_pipeline::condition_type::or_else);
    add_command(cp2.pipeline(), L("pipeline 2"));
    add_command(cp2.pipeline(), L("pipeline 3"));
    add_command(cp2.pipeline(), L("pipeline 4"));
    CHECK(cp2.condition() == conditional_pipeline::condition_type::or_else);
    CHECK(cp2.pipeline().commands().size() == 3);

    conditional_pipeline cp3(
            conditional_pipeline::condition_type::and_then,
            nullptr);
    add_command(cp3.pipeline(), L("pipeline 5"));
    CHECK(cp3.condition() == conditional_pipeline::condition_type::and_then);
    CHECK(cp3.pipeline().commands().size() == 1);

    conditional_pipeline cp4(
            conditional_pipeline::condition_type::or_else,
            nullptr);
    add_command(cp4.pipeline(), L("pipeline 6"));
    add_command(cp4.pipeline(), L("pipeline 7"));
    CHECK(cp4.condition() == conditional_pipeline::condition_type::or_else);
    CHECK(cp4.pipeline().commands().size() == 2);
}

TEST_CASE("Conditional pipeline print") {
    printer ps(printer::line_mode_type::single_line);
    printer pm(printer::line_mode_type::multi_line);

    conditional_pipeline cp1(
            conditional_pipeline::condition_type::and_then,
            new_pipeline(L("pipeline 1")));

    ps.indent_level() = 1;
    pm.indent_level() = 1;
    ps << cp1;
    pm << cp1;
    CHECK(ps.to_string() == L("&& ! pipeline 1"));
    CHECK(pm.to_string() == L("&&\n    ! pipeline 1"));

    conditional_pipeline cp2(
            conditional_pipeline::condition_type::or_else,
            new_pipeline(L("pipeline 2")));

    ps.indent_level() = 2;
    pm.indent_level() = 2;
    ps << cp2;
    pm << cp2;
    CHECK(ps.to_string() == L("&& ! pipeline 1 || ! pipeline 2"));
    CHECK(pm.to_string() ==
            L("&&\n    ! pipeline 1 ||\n        ! pipeline 2"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
