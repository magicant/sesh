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

#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/and_or_list.hh"
#include "language/syntax/command.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/printer.hh"
#include "language/syntax/sequence.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::and_or_list;
using sesh::language::syntax::command;
using sesh::language::syntax::conditional_pipeline;
using sesh::language::syntax::pipeline;
using sesh::language::syntax::printer;
using sesh::language::syntax::sequence;

struct command_stub : public command {
    xstring s;
    command_stub(xstring s) : command(), s(s) { }
    void print(printer &p) const override {
        p << s;
        p.delayed_characters() << L(' ');
    }
};

struct print_fixture {
    // C1 && C2; ! C3 | C4& C5
    sequence s;
    print_fixture() : s() {
        s.and_or_lists().emplace_back(
                sequence::and_or_list_pointer(new and_or_list(pipeline())));
        s.and_or_lists()[0]->first().commands().emplace_back(
                pipeline::command_pointer(new command_stub(L("C1"))));
        s.and_or_lists()[0]->rest().emplace_back(
                conditional_pipeline::condition_type::and_then);
        s.and_or_lists()[0]->rest()[0].pipeline().commands().emplace_back(
                pipeline::command_pointer(new command_stub(L("C2"))));

        s.and_or_lists().emplace_back(
                sequence::and_or_list_pointer(new and_or_list(pipeline())));
        s.and_or_lists()[1]->synchronicity() =
                and_or_list::synchronicity_type::asynchronous;
        s.and_or_lists()[1]->first().exit_status_mode() =
                pipeline::exit_status_mode_type::negated;
        s.and_or_lists()[1]->first().commands().emplace_back(
                pipeline::command_pointer(new command_stub(L("C3"))));
        s.and_or_lists()[1]->first().commands().emplace_back(
                pipeline::command_pointer(new command_stub(L("C4"))));

        s.and_or_lists().emplace_back(
                sequence::and_or_list_pointer(new and_or_list(pipeline())));
        s.and_or_lists()[2]->first().commands().emplace_back(
                pipeline::command_pointer(new command_stub(L("C5"))));
    }
};

TEST_CASE_METHOD(print_fixture, "Sequence print single-line") {
    printer p(printer::line_mode_type::single_line);
    p.delayed_characters() << L('X');
    p.delayed_lines() << L("Y\n");
    p.indent_level() = 2;
    p << s;
    CHECK(p.to_string() == L("XC1 && C2; ! C3 | C4& C5"));
    p.commit_delayed_characters();
    CHECK(p.to_string() == L("XC1 && C2; ! C3 | C4& C5; "));
}

TEST_CASE_METHOD(print_fixture, "Sequence print multi-line") {
    printer p(printer::line_mode_type::multi_line);
    p.delayed_characters() << L('X');
    p.delayed_lines() << L("Y\n");
    p.indent_level() = 2;
    p << s;
    CHECK(p.to_string() ==
            L("XC1 &&\nY\n        C2\n        ! C3 | C4&\n        C5"));
    p.commit_delayed_characters();
    CHECK(p.to_string() ==
            L("XC1 &&\nY\n        C2\n        ! C3 | C4&\n        C5; "));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */