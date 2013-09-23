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

#include "Skipper.hh"
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"

using sesh::common::Char;
using sesh::common::CharTraits;

namespace sesh {
namespace language {
namespace parser {

Skipper::Skipper(
        Environment &e,
        Predicate<Char> &&isStopper,
        LineContinuationTreatment lct) :
        Parser(e),
        mIsStopper(std::move(isStopper)),
        mLineContinuationTreatment(lct) {
    if (mIsStopper == nullptr)
        mIsStopper = [](Environment &, Char) { return false; };
}

void Skipper::removeLineContinuation() {
    switch (mLineContinuationTreatment) {
    case LineContinuationTreatment::LITERAL:
        return;
    case LineContinuationTreatment::REMOVE:
        environment().removeLineContinuation(environment().current());
        return;
    }
}

bool Skipper::currentIsStopper() const {
    CharInt ci = currentCharInt();
    return CharTraits::eq_int_type(ci, CharTraits::eof()) ||
            mIsStopper(environment(), CharTraits::to_char_type(ci));
}

void Skipper::skip() {
    while (removeLineContinuation(), !currentIsStopper())
        ++environment().current();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
