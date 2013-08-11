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

#ifndef INCLUDED_language_SimpleCommand_hh
#define INCLUDED_language_SimpleCommand_hh

#include <memory>
#include <vector>
#include "language/Assignment.hh"
#include "language/Command.hh"
#include "language/Word.hh"

namespace sesh {
namespace language {

class Printer;

/**
 * A valid simple command contains at least one word or assignment. Each word
 * must contain at least one word component.
 */
class SimpleCommand : public Command {

public:

    using WordPointer = std::unique_ptr<Word>;
    using AssignmentPointer = std::unique_ptr<Assignment>;

private:

    std::vector<WordPointer> mWords;
    std::vector<AssignmentPointer> mAssignments;

public:

    explicit SimpleCommand(const SourceLocation &sl);
    explicit SimpleCommand(SourceLocation &&sl);

    SimpleCommand(const SimpleCommand &) = delete;
    SimpleCommand(SimpleCommand &&) = default;
    SimpleCommand &operator=(const SimpleCommand &) = delete;
    SimpleCommand &operator=(SimpleCommand &&) = default;
    ~SimpleCommand() override = default;

    std::vector<WordPointer> &words() noexcept {
        return mWords;
    }
    const std::vector<WordPointer> &words() const noexcept {
        return mWords;
    }

    std::vector<AssignmentPointer> &assignments() noexcept {
        return mAssignments;
    }
    const std::vector<AssignmentPointer> &assignments() const noexcept {
        return mAssignments;
    }

    void print(Printer &) const override;

}; // class SimpleCommand

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_SimpleCommand_hh

/* vim: set et sw=4 sts=4 tw=79: */
