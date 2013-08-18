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
#include "Pipeline.hh"
#include <utility>
#include <vector>
#include "language/Printer.hh"

namespace sesh {
namespace language {

Pipeline::Pipeline(ExitStatusType e) : mCommands(), mExitStatusType(e) { }

void Pipeline::print(Printer &p) const {
    switch (exitStatusType()) {
    case ExitStatusType::STRAIGHT:
        break;
    case ExitStatusType::NEGATED:
        p << L"! ";
        break;
    }

    bool isFirst = true;
    for (const CommandPointer &c : commands()) {
        if (!isFirst)
            p << L"| ";
        p << *c;
        isFirst = false;
    }
}

} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
