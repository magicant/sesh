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
#include "AndOrList.hh"

#include <utility>
#include "common/xchar.hh"
#include "language/syntax/Printer.hh"

namespace sesh {
namespace language {
namespace syntax {

AndOrList::AndOrList(Pipeline &&first, Synchronicity s) :
        mFirst(std::move(first)), mRest(), mSynchronicity(s) { }

namespace {

inline void printSeparator(AndOrList::Synchronicity s, Printer &p) {
    p.clearDelayedCharacters();
    switch (s) {
    case AndOrList::Synchronicity::SEQUENTIAL:
        p.delayedCharacters() << L(';');
        break;
    case AndOrList::Synchronicity::ASYNCHRONOUS:
        p << L('&');
        break;
    }
    p.delayedCharacters() << L(' ');
}

} // namespace

void AndOrList::print(Printer &p) const {
    p << first();
    for (const ConditionalPipeline &cp : rest())
        p << cp;
    printSeparator(synchronicity(), p);
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
