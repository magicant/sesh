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
#include "common/Char.hh"
#include "common/String.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Printer.hh"

namespace {

using sesh::common::Char;
using sesh::common::String;
using sesh::language::syntax::Command;
using sesh::language::syntax::Printer;

class CommandStub : public Command {
public:
    using Command::Command;
    void print(Printer &) const override;
};

void CommandStub::print(Printer &) const {
    CHECK(false);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
