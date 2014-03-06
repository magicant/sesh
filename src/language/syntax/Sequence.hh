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

#ifndef INCLUDED_language_syntax_Sequence_hh
#define INCLUDED_language_syntax_Sequence_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Printable.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A sequence, which is called a "list" in the POSIX standard, is a series of
 * one or more and-or lists. Despite that definition, an instance of this class
 * may contain no and-or lists. Users of this class must validate the number of
 * contained and-or lists.
 */
class Sequence : public Printable {

public:

    using AndOrListPointer = std::unique_ptr<AndOrList>;

private:

    std::vector<AndOrListPointer> mAndOrLists;

public:

    Sequence() = default;
    Sequence(const Sequence &) = delete;
    Sequence(Sequence &&) = default;
    Sequence &operator=(const Sequence &) = delete;
    Sequence &operator=(Sequence &&) = default;
    ~Sequence() override = default;

    std::vector<AndOrListPointer> &andOrLists() noexcept {
        return mAndOrLists;
    }
    const std::vector<AndOrListPointer> &andOrLists() const noexcept {
        return mAndOrLists;
    }

    void print(Printer &) const override;

}; // class Sequence

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Sequence_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
