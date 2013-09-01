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

#include "Assignment.hh"
#include <stdexcept>
#include <utility>
#include "language/syntax/Printer.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

using String = common::String;

void createWordIfNull(Assignment::WordPointer &w) {
    if (w == nullptr)
        w.reset(new Word);
}

} // namespace

Assignment::Assignment() : mVariableName(), mValue(new Word) { }

Assignment::Assignment(const String &variableName, WordPointer &&value) :
        mVariableName(variableName), mValue(std::move(value)) {
    createWordIfNull(mValue);
}

Assignment::Assignment(String &&variableName, WordPointer &&value) :
        mVariableName(std::move(variableName)), mValue(std::move(value)) {
    createWordIfNull(mValue);
}

void Assignment::print(Printer &p) const {
    p << variableName() << L'=' << value();
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */