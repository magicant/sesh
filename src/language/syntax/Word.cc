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
#include "Word.hh"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>
#include "common/Maybe.hh"
#include "common/String.hh"

using sesh::common::Maybe;
using sesh::common::String;
using sesh::common::createMaybeOf;

namespace sesh {
namespace language {
namespace syntax {

void Word::addComponent(ComponentPointer c) {
    mMaybeConstantValueCache.clear();

    assert(c != nullptr);
    mComponents.push_back(std::move(c));
}

void Word::append(Word &&w) {
    mMaybeConstantValueCache.clear();
    w.mMaybeConstantValueCache.clear();

    std::move(
            w.mComponents.begin(),
            w.mComponents.end(),
            std::back_inserter(mComponents));
    w.mComponents.clear();
}

Maybe<String> Word::computeMaybeConstantValue() const {
    String constantValue;
    for (const ComponentPointer &c : components())
        if (!c->appendConstantValue(constantValue))
            return Maybe<String>();
    return createMaybeOf(std::move(constantValue));
}

const Maybe<String> &Word::maybeConstantValue() const {
    if (!mMaybeConstantValueCache.hasValue())
        mMaybeConstantValueCache.emplace(computeMaybeConstantValue());
    return mMaybeConstantValueCache.value();
}

void Word::print(Printer &p) const {
    for (const ComponentPointer &c : components())
        p << *c;
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
