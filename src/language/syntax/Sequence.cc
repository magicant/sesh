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
#include "Sequence.hh"

#include "language/syntax/and_or_list.hh"
#include "language/syntax/printer.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

inline void printSeparator(Printer &p) {
    switch (p.lineMode()) {
    case Printer::LineMode::SINGLE_LINE:
        break;
    case Printer::LineMode::MULTI_LINE:
        p.breakLine();
        p.printIndent();
        break;
    }
}

} // namespace

void Sequence::print(Printer &p) const {
    bool isFirst = true;
    for (const AndOrListPointer &aol : andOrLists()) {
        if (!isFirst)
            printSeparator(p);
        p << *aol;
        isFirst = false;
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
