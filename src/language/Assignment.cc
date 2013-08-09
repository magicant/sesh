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
#include "Assignment.hh"
#include <stdexcept>
#include <utility>
#include "language/Printer.hh"
#include "language/Word.hh"

namespace sesh {
namespace language {

namespace {

void createWordIfNull(Assignment::WordPointer &w) {
    if (w == nullptr)
        w.reset(new Word);
}

} // namespace

Assignment::Assignment() : mVariableName(), mValue(new Word) { }

Assignment::Assignment(const std::wstring &variableName, WordPointer &&value) :
        mVariableName(variableName), mValue(std::move(value)) {
    createWordIfNull(mValue);
}

Assignment::Assignment(std::wstring &&variableName, WordPointer &&value) :
        mVariableName(std::move(variableName)), mValue(std::move(value)) {
    createWordIfNull(mValue);
}

void Assignment::print(Printer &p) const {
    p << variableName() << L'=' << value();
}

} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79: */