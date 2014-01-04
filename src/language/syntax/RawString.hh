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

#ifndef INCLUDED_language_syntax_RawString_hh
#define INCLUDED_language_syntax_RawString_hh

#include "buildconfig.h"

#include <utility>
#include "common/String.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A raw string is a word component that is an unquoted string literal.
 *
 * The value of a raw string is not expected to contain characters that have
 * special meanings in the shell syntax (e.g. semicolon and backslash), but the
 * implementation of this class does not itself check the validity of the
 * string value.
 */
class RawString : public WordComponent {

private:

    common::String mValue;

public:

    RawString() : mValue() { }
    explicit RawString(const common::String &s) : mValue(s) { }
    explicit RawString(common::String &&s) noexcept : mValue(std::move(s)) { }

    RawString(const RawString &) = default;
    RawString(RawString &&) = default;
    RawString &operator=(const RawString &) = default;
    RawString &operator=(RawString &&) = default;
    ~RawString() override = default;

    common::String &value() { return mValue; }
    const common::String &value() const { return mValue; }

    bool appendConstantValue(common::String &) const override;

    void print(Printer &) const override;

}; // class RawString

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_RawString_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
