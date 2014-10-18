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

#ifndef INCLUDED_language_syntax_sequence_hh
#define INCLUDED_language_syntax_sequence_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/and_or_list.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A sequence, which is called a "list" in the POSIX standard, is a series of
 * one or more and-or lists. Despite that definition, an instance of this class
 * may contain no and-or lists. Users of this class must validate the number of
 * contained and-or lists.
 */
class sequence {

public:

    using and_or_list_pointer = std::unique_ptr<and_or_list>;

private:

    std::vector<and_or_list_pointer> m_and_or_lists;

public:

    sequence() = default;
    sequence(const sequence &) = delete;
    sequence(sequence &&) = default;
    sequence &operator=(const sequence &) = delete;
    sequence &operator=(sequence &&) = default;
    ~sequence() = default;

    std::vector<and_or_list_pointer> &and_or_lists() noexcept {
        return m_and_or_lists;
    }
    const std::vector<and_or_list_pointer> &and_or_lists() const noexcept {
        return m_and_or_lists;
    }

}; // class sequence

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_sequence_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
