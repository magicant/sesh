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

#include "buildconfig.h"
#include "Operator.hh"

#include "common/Char.hh"
#include "common/String.hh"
#include "common/Trie.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::common::Trie;

namespace sesh {
namespace language {
namespace parser {

const String Operator::AND = L("&");
const String Operator::AND_AND = L("&&");
const String Operator::GREATER = L(">");
const String Operator::GREATER_AND = L(">&");
const String Operator::GREATER_GREATER = L(">>");
const String Operator::GREATER_PIPE = L(">|");
const String Operator::LEFT_PARENTHESIS = L("(");
const String Operator::LESS = L("<");
const String Operator::LESS_AND = L("<&");
const String Operator::LESS_GREATER = L("<>");
const String Operator::LESS_LESS = L("<<");
const String Operator::LESS_LESS_MINUS = L("<<-");
const String Operator::LESS_PIPE = L("<|");
const String Operator::PIPE = L("|");
const String Operator::PIPE_PIPE = L("||");
const String Operator::RIGHT_PARENTHESIS = L(")");
const String Operator::SEMICOLON = L(";");
const String Operator::SEMICOLON_SEMICOLON = L(";;");

Trie<Char, Operator> Operator::createTrie() {
    Trie<Char, Operator> trie;
    for (Operator o : {
            operatorAnd(),
            operatorAndAnd(),
            operatorGreater(),
            operatorGreaterAnd(),
            operatorGreaterGreater(),
            operatorGreaterPipe(),
            operatorLeftParenthesis(),
            operatorLess(),
            operatorLessAnd(),
            operatorLessGreater(),
            operatorLessLess(),
            operatorLessLessMinus(),
            operatorLessPipe(),
            operatorPipe(),
            operatorPipePipe(),
            operatorRightParenthesis(),
            operatorSemicolon(),
            operatorSemicolonSemicolon(),
            })
        trie.emplaceDescendants(o.get()).emplaceValue(o);
    return std::move(trie);
}

const Trie<Char, Operator> Operator::TRIE = createTrie();

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
