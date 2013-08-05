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

#include "common.hh"
#include "SimpleCommand.hh"
#include <utility>
#include "language/Printer.hh"

namespace sesh {
namespace language {

SimpleCommand::SimpleCommand(const SourceLocation &sl) :
        Command(sl), mWords(), mAssignments() { }

SimpleCommand::SimpleCommand(SourceLocation &&sl) :
        Command(std::move(sl)), mWords(), mAssignments() { }

void SimpleCommand::print(Printer &p) const {
    for (const AssignmentPointer &a : assignments()) {
        p << *a;
        p.delayedCharacters() << L' ';
    }
    for (const WordPointer &w : words()) {
        p << *w;
        p.delayedCharacters() << L' ';
    }
}

} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79: */
