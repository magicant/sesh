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

#ifndef INCLUDED_language_syntax_simple_command_hh
#define INCLUDED_language_syntax_simple_command_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/assignment.hh"
#include "language/syntax/command.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A valid simple command contains at least one word, assignment, or
 * redirection. Each word must contain at least one word component.
 */
class simple_command : public command {

public:

    using word_pointer = std::unique_ptr<Word>;
    using assignment_pointer = std::unique_ptr<assignment>;

private:

    std::vector<word_pointer> m_words;
    std::vector<assignment_pointer> m_assignments;

public:

    simple_command();

    simple_command(const simple_command &) = delete;
    simple_command(simple_command &&) = default;
    simple_command &operator=(const simple_command &) = delete;
    simple_command &operator=(simple_command &&) = default;
    ~simple_command() override = default;

    std::vector<word_pointer> &words() noexcept {
        return m_words;
    }
    const std::vector<word_pointer> &words() const noexcept {
        return m_words;
    }

    std::vector<assignment_pointer> &assignments() noexcept {
        return m_assignments;
    }
    const std::vector<assignment_pointer> &assignments() const noexcept {
        return m_assignments;
    }

    void print(printer &) const override;

}; // class simple_command

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_simple_command_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
