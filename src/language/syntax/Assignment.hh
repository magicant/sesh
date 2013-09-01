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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_syntax_Assignment_hh
#define INCLUDED_language_syntax_Assignment_hh

#include <memory>
#include "common/String.hh"
#include "language/syntax/Printable.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace syntax {

class Printer;

/** An assignment is part of a simple command. */
class Assignment : public Printable {

public:

    using WordPointer = std::unique_ptr<Word>;

private:

    common::String mVariableName;
    WordPointer mValue;

public:

    Assignment();
    Assignment(const common::String &variableName, WordPointer &&value);
    Assignment(common::String &&variableName, WordPointer &&value);

    Assignment(const Assignment &) = delete;
    Assignment(Assignment &&) = default;
    Assignment &operator=(const Assignment &) = delete;
    Assignment &operator=(Assignment &&) = default;
    ~Assignment() override = default;

    common::String &variableName() noexcept {
        return mVariableName;
    }
    const common::String &variableName() const noexcept {
        return mVariableName;
    }

    Word &value() noexcept { return *mValue; }
    const Word &value() const noexcept { return *mValue; }

    void print(Printer &) const override;

}; // class Assignment

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Assignment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
