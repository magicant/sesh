/* Copyright (C) 2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_Operator_hh
#define INCLUDED_language_parser_Operator_hh

#include "buildconfig.h"

#include "common/Char.hh"
#include "common/Reference.hh"
#include "common/String.hh"
#include "common/Trie.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A wrapper that holds a reference to an operator string. The referred-to
 * string is one of the static constant member strings of this class.
 */
class Operator : public common::Reference<const common::String> {

public:

    static const common::String AND;
    static const common::String AND_AND;
    static const common::String GREATER;
    static const common::String GREATER_AND;
    static const common::String GREATER_GREATER;
    static const common::String GREATER_PIPE;
    static const common::String LEFT_PARENTHESIS;
    static const common::String LESS;
    static const common::String LESS_AND;
    static const common::String LESS_GREATER;
    static const common::String LESS_LESS;
    static const common::String LESS_LESS_MINUS;
    static const common::String LESS_PIPE;
    static const common::String PIPE;
    static const common::String PIPE_PIPE;
    static const common::String RIGHT_PARENTHESIS;
    static const common::String SEMICOLON;
    static const common::String SEMICOLON_SEMICOLON;

private:

    explicit Operator(const common::String &value) noexcept :
            Reference(value) { }

    static common::Trie<common::Char, Operator> createTrie();

public:

    static Operator operatorAnd() noexcept {
        return Operator(AND);
    }
    static Operator operatorAndAnd() noexcept {
        return Operator(AND_AND);
    }
    static Operator operatorGreater() noexcept {
        return Operator(GREATER);
    }
    static Operator operatorGreaterAnd() noexcept {
        return Operator(GREATER_AND);
    }
    static Operator operatorGreaterGreater() noexcept {
        return Operator(GREATER_GREATER);
    }
    static Operator operatorGreaterPipe() noexcept {
        return Operator(GREATER_PIPE);
    }
    static Operator operatorLeftParenthesis() noexcept {
        return Operator(LEFT_PARENTHESIS);
    }
    static Operator operatorLess() noexcept {
        return Operator(LESS);
    }
    static Operator operatorLessAnd() noexcept {
        return Operator(LESS_AND);
    }
    static Operator operatorLessGreater() noexcept {
        return Operator(LESS_GREATER);
    }
    static Operator operatorLessLess() noexcept {
        return Operator(LESS_LESS);
    }
    static Operator operatorLessLessMinus() noexcept {
        return Operator(LESS_LESS_MINUS);
    }
    static Operator operatorLessPipe() noexcept {
        return Operator(LESS_PIPE);
    }
    static Operator operatorPipe() noexcept {
        return Operator(PIPE);
    }
    static Operator operatorPipePipe() noexcept {
        return Operator(PIPE_PIPE);
    }
    static Operator operatorRightParenthesis() noexcept {
        return Operator(RIGHT_PARENTHESIS);
    }
    static Operator operatorSemicolon() noexcept {
        return Operator(SEMICOLON);
    }
    static Operator operatorSemicolonSemicolon() noexcept {
        return Operator(SEMICOLON_SEMICOLON);
    }

    static const common::Trie<common::Char, Operator> TRIE;

}; // class Operator

inline bool operator==(const Operator &o1, const Operator &o2) noexcept {
    return &o1.get() == &o2.get();
}

inline bool operator!=(const Operator &o1, const Operator &o2) noexcept {
    return !(o1 == o2);
}

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Operator_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
