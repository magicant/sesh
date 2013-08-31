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

#ifndef INCLUDED_language_Command_hh
#define INCLUDED_language_Command_hh

#include <utility>
#include "language/Printable.hh"
#include "parser/SourceLocation.hh"

namespace sesh {
namespace language {

class Printer;

/** A command is either a simple or compound command. */
class Command : public Printable {

private:

    parser::SourceLocation mSourceLocation;

    // TODO Redirection

public:

    explicit Command(const parser::SourceLocation &sl) :
            mSourceLocation(sl) { }
    explicit Command(parser::SourceLocation &&sl) :
            mSourceLocation(std::move(sl)) { }

    Command(const Command &) = default;
    Command(Command &&) = default;
    Command &operator=(const Command &) = default;
    Command &operator=(Command &&) = default;
    ~Command() override = default;

    parser::SourceLocation &sourceLocation() noexcept {
        return mSourceLocation;
    }
    const parser::SourceLocation &sourceLocation() const noexcept {
        return mSourceLocation;
    }

}; // class Command

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_Command_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
