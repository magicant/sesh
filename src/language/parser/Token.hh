/* Copyright (C) 2013-2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_Token_hh
#define INCLUDED_language_parser_Token_hh

#include "buildconfig.h"

#include <memory>
#include "common/Variant.hh"
#include "language/parser/Keyword.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"

namespace sesh {

namespace language {
namespace parser {

using Token = common::Variant<
        std::unique_ptr<syntax::Word>,
        std::unique_ptr<syntax::Assignment>,
        Keyword>;

enum class TokenType { WORD, ASSIGNMENT, KEYWORD, };

} // namespace parser
} // namespace language

namespace common {

template<>
class EnumTraits<language::parser::TokenType> {
public:
    constexpr static language::parser::TokenType max =
            language::parser::TokenType::KEYWORD;
};

} // namespace common

} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Token_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
