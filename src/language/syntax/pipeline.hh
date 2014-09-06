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

#ifndef INCLUDED_language_syntax_pipeline_hh
#define INCLUDED_language_syntax_pipeline_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/command.hh"
#include "language/syntax/Printable.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A pipeline is a list of one or more commands that are executed at a time
 * with the standard input/output connected with each other.
 */
class pipeline : public Printable {

public:

    using command_pointer = std::unique_ptr<command>;

    enum class exit_status_mode_type {
        straight,
        negated,
    };

private:

    std::vector<command_pointer> m_commands;
    exit_status_mode_type m_exit_status_mode;

public:

    explicit pipeline(exit_status_mode_type = exit_status_mode_type::straight);

    pipeline(const pipeline &) = delete;
    pipeline(pipeline &&) = default;
    pipeline &operator=(const pipeline &) = delete;
    pipeline &operator=(pipeline &&) = default;
    ~pipeline() override = default;

    std::vector<command_pointer> &commands() noexcept {
        return m_commands;
    }
    const std::vector<command_pointer> &commands() const noexcept {
        return m_commands;
    }

    exit_status_mode_type &exit_status_mode() noexcept {
        return m_exit_status_mode;
    }
    exit_status_mode_type exit_status_mode() const noexcept {
        return m_exit_status_mode;
    }

    void print(Printer &) const override;

}; // class pipeline

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_pipeline_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
