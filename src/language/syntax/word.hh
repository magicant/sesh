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

#ifndef INCLUDED_language_syntax_word_hh
#define INCLUDED_language_syntax_word_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "common/either.hh"
#include "common/xstring.hh"
#include "language/syntax/printable.hh"
#include "language/syntax/word_component.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A word is a token that may contain expansions. A word is composed of any
 * number of word components.
 */
class word : public printable {

public:

    using component_pointer = std::unique_ptr<word_component>;

private:

    std::vector<component_pointer> m_components;

    mutable common::maybe<common::maybe<common::xstring>>
            m_maybe_constant_value_cache;

public:

    template<typename... Arg>
    word(Arg &&... arg) :
            m_components(std::forward<Arg>(arg)...),
            m_maybe_constant_value_cache() { }

    word() = default;
    word(const word &) = delete;
    word(word &&) = default;
    word &operator=(const word &) = delete;
    word &operator=(word &&) = default;
    ~word() override = default;

    const std::vector<component_pointer> &components() const noexcept {
        return m_components;
    }

    /**
     * Adds a component to this word.
     * @param c non-null pointer to the component to add.
     */
    void add_component(component_pointer c);

    /**
     * Moves all components of the argument word to the end of this word. The
     * argument word will be empty after this method returns.
     */
    void append(word &&);

private:

    common::maybe<common::xstring> compute_maybe_constant_value() const;

public:

    /**
     * If the value of this word is constant, i.e., this word always evaluates
     * to the same single string regardless of the execution environment, then
     * returns a reference to a maybe object containing the constant value.
     * Otherwise, returns a reference to an empty maybe object.
     */
    const common::maybe<common::xstring> &maybe_constant_value() const;

    /** Returns true if all components of this word are raw strings. */
    bool is_raw_string() const;

    void print(printer &) const override;

}; // class word

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_word_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
