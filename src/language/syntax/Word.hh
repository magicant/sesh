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
#include <utility>
#include <vector>
#include "common/maybe.hh"
#include "common/xstring.hh"
#include "language/syntax/printable.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A word is a token that may contain expansions. A word is composed of any
 * number of word components.
 */
class Word : public printable {

public:

    using ComponentPointer = std::unique_ptr<WordComponent>;

private:

    std::vector<ComponentPointer> mComponents;

    mutable common::maybe<common::maybe<common::xstring>>
            mMaybeConstantValueCache;

public:

    template<typename... Arg>
    Word(Arg &&... arg) :
            mComponents(std::forward<Arg>(arg)...),
            mMaybeConstantValueCache() { }

    Word() = default;
    Word(const Word &) = delete;
    Word(Word &&) = default;
    Word &operator=(const Word &) = delete;
    Word &operator=(Word &&) = default;
    ~Word() override = default;

    const std::vector<ComponentPointer> &components() const noexcept {
        return mComponents;
    }

    /**
     * Adds a component to this word.
     * @param c non-null pointer to the component to add.
     */
    void addComponent(ComponentPointer c);

    /**
     * Moves all components of the argument word to the end of this word. The
     * argument word will be empty after this method returns.
     */
    void append(Word &&);

private:

    common::maybe<common::xstring> computeMaybeConstantValue() const;

public:

    /**
     * If the value of this word is constant, i.e., this word always evaluates
     * to the same single string regardless of the execution environment, then
     * returns a reference to a maybe object containing the constant value.
     * Otherwise, returns a reference to an empty maybe object.
     */
    const common::maybe<common::xstring> &maybeConstantValue() const;

    /** Returns true if all components of this word are raw strings. */
    bool isRawString() const;

    void print(Printer &) const override;

}; // class Word

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Word_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
