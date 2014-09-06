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

#ifndef INCLUDED_language_syntax_assignment_hh
#define INCLUDED_language_syntax_assignment_hh

#include "buildconfig.h"

#include <memory>
#include "common/xstring.hh"
#include "language/syntax/printable.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace syntax {

/** An assignment is part of a simple command. */
class assignment : public printable {

public:

    using word_pointer = std::unique_ptr<Word>;

private:

    common::xstring m_variable_name;
    word_pointer m_value;

public:

    assignment();
    assignment(const common::xstring &variable_name, word_pointer &&value);
    assignment(common::xstring &&variable_name, word_pointer &&value);

    assignment(const assignment &) = delete;
    assignment(assignment &&) = default;
    assignment &operator=(const assignment &) = delete;
    assignment &operator=(assignment &&) = default;
    ~assignment() override = default;

    common::xstring &variable_name() noexcept {
        return m_variable_name;
    }
    const common::xstring &variable_name() const noexcept {
        return m_variable_name;
    }

    Word &value() noexcept { return *m_value; }
    const Word &value() const noexcept { return *m_value; }

    void print(Printer &) const override;

}; // class assignment

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_assignment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
