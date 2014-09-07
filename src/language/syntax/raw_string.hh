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

#ifndef INCLUDED_language_syntax_raw_string_hh
#define INCLUDED_language_syntax_raw_string_hh

#include "buildconfig.h"

#include <utility>
#include "common/xstring.hh"
#include "language/syntax/word_component.hh"

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
class raw_string : public word_component {

private:

    common::xstring m_value;

public:

    raw_string() : m_value() { }
    explicit raw_string(const common::xstring &s) : m_value(s) { }
    explicit raw_string(common::xstring &&s) noexcept :
            m_value(std::move(s)) { }

    raw_string(const raw_string &) = default;
    raw_string(raw_string &&) = default;
    raw_string &operator=(const raw_string &) = default;
    raw_string &operator=(raw_string &&) = default;
    ~raw_string() override = default;

    bool is_raw_string() const noexcept override { return true; }

    common::xstring &value() { return m_value; }
    const common::xstring &value() const { return m_value; }

    bool append_constant_value(common::xstring &) const override;

    void print(printer &) const override;

}; // class raw_string

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_raw_string_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
