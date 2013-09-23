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

#ifndef INCLUDED_language_syntax_Word_hh
#define INCLUDED_language_syntax_Word_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include "language/syntax/Printable.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace syntax {

class Printer;

/**
 * A word is a token that may contain expansions. A word is composed of any
 * number of word components.
 */
class Word : public Printable {

public:

    using ComponentPointer = std::unique_ptr<WordComponent>;

private:

    std::vector<ComponentPointer> mComponents;

public:

    Word() = default;
    Word(const Word &) = delete;
    Word(Word &&) = default;
    Word &operator=(const Word &) = delete;
    Word &operator=(Word &&) = default;
    ~Word() override = default;

    std::vector<ComponentPointer> &components() noexcept {
        return mComponents;
    }
    const std::vector<ComponentPointer> &components() const noexcept {
        return mComponents;
    }

    void print(Printer &) const override;

}; // class Word

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Word_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
