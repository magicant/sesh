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

#include <cstddef>
#include <memory>
#include "common/xchar.hh"
#include "common/xstring.hh"
#include "language/syntax/command.hh"
#include "language/syntax/printer.hh"

namespace {

using sesh::common::xstring;
using sesh::language::syntax::command;
using sesh::language::syntax::printer;

class command_stub : public command {
public:
    using command::command;
    void print(printer &) const override;
};

void command_stub::print(printer &) const {
    CHECK(false);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
