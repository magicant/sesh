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
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/printer_test_helper.hh"

namespace {

using sesh::common::xchar;
using sesh::common::xstring;
using sesh::language::syntax::command;
using sesh::language::syntax::for_each_line_mode;
using sesh::language::syntax::pipeline;
using sesh::language::syntax::printer;

class command_stub : public command {
private:
    xstring m_string;
public:
    explicit command_stub(const xchar *s) : command(), m_string(s) { }
    void print(printer &p) const override {
        p << m_string;
        p.delayed_characters() << L(' ');
    }
};

void add_command(pipeline &p, const xchar *s) {
    p.commands().push_back(pipeline::command_pointer(new command_stub(s)));
}

void check_for_each_line_mode(const pipeline &pl, const xstring &expected) {
    for_each_line_mode([&pl, &expected](printer &p) {
        p << pl;
        CHECK(p.to_string() == expected);
    });
}

TEST_CASE("Pipeline print") {
    pipeline pl;

    add_command(pl, L("Command 1"));

    check_for_each_line_mode(pl, L("Command 1"));

    CHECK(pl.exit_status_mode() == pipeline::exit_status_mode_type::straight);
    pl.exit_status_mode() = pipeline::exit_status_mode_type::negated;

    check_for_each_line_mode(pl, L("! Command 1"));

    CHECK(pl.exit_status_mode() == pipeline::exit_status_mode_type::negated);
    add_command(pl, L("Command 2"));

    check_for_each_line_mode(pl, L("! Command 1 | Command 2"));

    CHECK(pl.exit_status_mode() == pipeline::exit_status_mode_type::negated);
    pl.exit_status_mode() = pipeline::exit_status_mode_type::straight;

    check_for_each_line_mode(pl, L("Command 1 | Command 2"));

    CHECK(pl.exit_status_mode() == pipeline::exit_status_mode_type::straight);
    add_command(pl, L("The last command"));

    check_for_each_line_mode(
            pl, L("Command 1 | Command 2 | The last command"));

    CHECK(pl.exit_status_mode() == pipeline::exit_status_mode_type::straight);
    pl.exit_status_mode() = pipeline::exit_status_mode_type::negated;

    check_for_each_line_mode(
            pl, L("! Command 1 | Command 2 | The last command"));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
