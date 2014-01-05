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
#include "Keyword.hh"

#include "common/Char.hh"
#include "common/Maybe.hh"
#include "common/String.hh"

using sesh::common::Maybe;
using sesh::common::String;

namespace sesh {
namespace language {
namespace parser {

const String Keyword::CASE = L("case");
const String Keyword::DO = L("do");
const String Keyword::DONE = L("done");
const String Keyword::ELIF = L("elif");
const String Keyword::ELSE = L("else");
const String Keyword::ESAC = L("esac");
const String Keyword::EXCLAMATION = L("!");
const String Keyword::FI = L("fi");
const String Keyword::FOR = L("for");
const String Keyword::FUNCTION = L("function");
const String Keyword::IF = L("if");
const String Keyword::IN = L("in");
const String Keyword::LEFT_BRACE = L("{");
const String Keyword::LEFT_BRACKET_BRACKET = L("[[");
const String Keyword::RIGHT_BRACE = L("}");
const String Keyword::RIGHT_BRACKET_BRACKET = L("]]");
const String Keyword::SELECT = L("select");
const String Keyword::THEN = L("then");
const String Keyword::UNTIL = L("until");
const String Keyword::WHILE = L("while");

Maybe<Keyword> Keyword::parse(const String &s) {
    // TODO Use trie for simpler code?

    if (s.empty())
        return Maybe<Keyword>();

    switch (s[0]) {
    case L('!'):
        if (s.length() == 1)
            return Keyword(EXCLAMATION);
        break;
    case L('{'):
        if (s.length() == 1)
            return Keyword(LEFT_BRACE);
        break;
    case L('}'):
        if (s.length() == 1)
            return Keyword(RIGHT_BRACE);
        break;
    case L('['):
        if (s == LEFT_BRACKET_BRACKET)
            return Keyword(LEFT_BRACKET_BRACKET);
        break;
    case L(']'):
        if (s == RIGHT_BRACKET_BRACKET)
            return Keyword(RIGHT_BRACKET_BRACKET);
        break;
    case L('c'):
        if (s == CASE)
            return Keyword(CASE);
        break;
    case L('d'):
        if (s == DO)
            return Keyword(DO);
        if (s == DONE)
            return Keyword(DONE);
        break;
    case L('e'):
        if (s == ELIF)
            return Keyword(ELIF);
        if (s == ELSE)
            return Keyword(ELSE);
        if (s == ESAC)
            return Keyword(ESAC);
        break;
    case L('f'):
        if (s == FI)
            return Keyword(FI);
        if (s == FOR)
            return Keyword(FOR);
        if (s == FUNCTION)
            return Keyword(FUNCTION);
        break;
    case L('i'):
        if (s == IF)
            return Keyword(IF);
        if (s == IN)
            return Keyword(IN);
        break;
    case L('s'):
        if (s == SELECT)
            return Keyword(SELECT);
        break;
    case L('t'):
        if (s == THEN)
            return Keyword(THEN);
        break;
    case L('u'):
        if (s == UNTIL)
            return Keyword(UNTIL);
        break;
    case L('w'):
        if (s == WHILE)
            return Keyword(WHILE);
        break;
    }

    return Maybe<Keyword>();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
