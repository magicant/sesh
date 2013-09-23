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

#ifndef INCLUDED_language_syntax_Pipeline_hh
#define INCLUDED_language_syntax_Pipeline_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/Command.hh"
#include "language/syntax/Printable.hh"

namespace sesh {
namespace language {
namespace syntax {

class Printer;

/**
 * A pipeline is a list of one or more commands that are executed at a time
 * with the standard input/output connected with each other.
 */
class Pipeline : public Printable {

public:

    using CommandPointer = std::unique_ptr<Command>;

    enum class ExitStatusType {
        STRAIGHT,
        NEGATED,
    };

private:

    std::vector<CommandPointer> mCommands;
    ExitStatusType mExitStatusType;

public:

    explicit Pipeline(ExitStatusType = ExitStatusType::STRAIGHT);

    Pipeline(const Pipeline &) = delete;
    Pipeline(Pipeline &&) = default;
    Pipeline &operator=(const Pipeline &) = delete;
    Pipeline &operator=(Pipeline &&) = default;
    ~Pipeline() override = default;

    std::vector<CommandPointer> &commands() noexcept {
        return mCommands;
    }
    const std::vector<CommandPointer> &commands() const noexcept {
        return mCommands;
    }

    ExitStatusType &exitStatusType() noexcept { return mExitStatusType; }
    ExitStatusType exitStatusType() const noexcept { return mExitStatusType; }

    void print(Printer &) const override;

}; // class Pipeline

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Pipeline_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
