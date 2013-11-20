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

#include "buildconfig.h"
#include "token.hh"

#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Predicate.hh"
#include "language/source/SourceBuffer.hh"

using sesh::common::CharTraits;
using sesh::common::String;
using sesh::language::source::SourceBuffer;
using Size = sesh::language::source::SourceBuffer::Size;
using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

namespace sesh {
namespace language {
namespace parser {

namespace token {

const String AND = L("&");
const String AND_AND = L("&&");
const String GREATER = L(">");
const String GREATER_AND = L(">&");
const String GREATER_GREATER = L(">>");
const String GREATER_PIPE = L(">|");
const String LEFT_PARENTHESIS = L("(");
const String LESS = L("<");
const String LESS_AND = L("<&");
const String LESS_GREATER = L("<>");
const String LESS_LESS = L("<<");
const String LESS_LESS_MINUS = L("<<-");
const String LESS_PIPE = L("<|");
const String PIPE = L("|");
const String PIPE_PIPE = L("||");
const String RIGHT_PARENTHESIS = L(")");
const String SEMICOLON = L(";");
const String SEMICOLON_SEMICOLON = L(";;");

const String CASE = L("case");
const String DO = L("do");
const String DONE = L("done");
const String ELIF = L("elif");
const String ELSE = L("else");
const String ESAC = L("esac");
const String EXCLAMATION = L("!");
const String FI = L("fi");
const String FOR = L("for");
const String FUNCTION = L("function");
const String IF = L("if");
const String IN = L("in");
const String LEFT_BRACE = L("{");
const String LEFT_BRACKET_BRACKET = L("[[");
const String RIGHT_BRACE = L("}");
const String RIGHT_BRACKET_BRACKET = L("]]");
const String SELECT = L("select");
const String THEN = L("then");
const String UNTIL = L("until");
const String WHILE = L("while");

} // namespace token

namespace {

const String EMPTY;

/** Remove line continuations if any and return the current character. */
CharTraits::int_type charAt(SourceBuffer::Difference n, Environment &e) {
    auto i = e.current() + n;
    while (e.removeLineContinuation(i)) { }
    return dereference(e, i);
}

} // namespace

#define IL(x) (CharTraits::to_int_type(L(x)))

const String &peekSymbol(Environment &e) {
    switch (charAt(0, e)) {
    case IL('&'):
        switch (charAt(1, e)) {
        case IL('&'):
            return token::AND_AND;
        default:
            return token::AND;
        }
    case IL('|'):
        switch (charAt(1, e)) {
        case IL('|'):
            return token::PIPE_PIPE;
        default:
            return token::PIPE;
        }
    case IL(';'):
        switch (charAt(1, e)) {
        case IL(';'):
            return token::SEMICOLON_SEMICOLON;
        default:
            return token::SEMICOLON;
        }
    case IL('<'):
        switch (charAt(1, e)) {
        case IL('<'):
            switch (charAt(2, e)) {
            case IL('-'):
                return token::LESS_LESS_MINUS;
            default:
                return token::LESS_LESS;
            }
        case IL('>'):
            return token::LESS_GREATER;
        case IL('&'):
            return token::LESS_AND;
        case IL('|'):
            return token::LESS_PIPE;
        default:
            return token::LESS;
        }
    case IL('>'):
        switch (charAt(1, e)) {
        case IL('>'):
            return token::GREATER_GREATER;
        case IL('&'):
            return token::GREATER_AND;
        case IL('|'):
            return token::GREATER_PIPE;
        default:
            return token::GREATER;
        }
    case IL('('):
        return token::LEFT_PARENTHESIS;
    case IL(')'):
        return token::RIGHT_PARENTHESIS;
    default:
        return EMPTY;
    }
}

const String &parseSymbol(Environment &e) {
    const String &symbol = peekSymbol(e);
    e.current() += symbol.length();
    return symbol;
}

namespace {

/**
 * Returns a candidate keyword that may exist at the current position.
 *
 * If there actually is a keyword at the current position, the keyword is
 * returned. Otherwise, the return value is an empty string or an arbitrary
 * keyword string.
 */
const String &keywordCandidate(Environment &e) {
    switch (charAt(0, e)) {
    case IL('!'):
        return token::EXCLAMATION;
    case IL('{'):
        return token::LEFT_BRACE;
    case IL('}'):
        return token::RIGHT_BRACE;
    case IL('['):
        return token::LEFT_BRACKET_BRACKET;
    case IL(']'):
        return token::RIGHT_BRACKET_BRACKET;
    case IL('c'):
        return token::CASE;
    case IL('d'):
        if (charAt(1, e) != IL('o'))
            return EMPTY;
        if (charAt(2, e) == IL('n'))
            return token::DONE;
        else
            return token::DO;
    case IL('e'):
        switch (charAt(1, e)) {
        case IL('l'):
            switch (charAt(2, e)) {
            case IL('i'):
                return token::ELIF;
            case IL('s'):
                return token::ELSE;
            default:
                return EMPTY;
            }
        case IL('s'):
            return token::ESAC;
        default:
            return EMPTY;
        }
        break;
    case IL('f'):
        switch (charAt(1, e)) {
        case IL('i'):
            return token::FI;
        case IL('o'):
            return token::FOR;
        case IL('u'):
            return token::FUNCTION;
        default:
            return EMPTY;
        }
    case IL('i'):
        switch (charAt(1, e)) {
        case IL('f'):
            return token::IF;
        case IL('n'):
            return token::IN;
        default:
            return EMPTY;
        }
    case IL('s'):
        return token::SELECT;
    case IL('t'):
        return token::THEN;
    case IL('u'):
        return token::UNTIL;
    case IL('w'):
        return token::WHILE;
    default:
        return EMPTY;
    }
}

} // namespace

const String &peekKeyword(Environment &e) {
    const String &keyword = keywordCandidate(e);

    if (keyword.empty())
        return keyword; // not a keyword

    // Check if the current token starts with the (candidate) keyword string
    for (String::size_type i = 0; i < keyword.length(); i++)
        if (charAt(i, e) != CharTraits::to_int_type(keyword[i]))
            return EMPTY;

    // A keyword must be followed by EOF or a token delimiter.
    auto charAfterKeyword = charAt(keyword.length(), e);
    if (!CharTraits::eq_int_type(charAfterKeyword, CharTraits::eof()) &&
            !isTokenDelimiter(e, CharTraits::to_char_type(charAfterKeyword)))
        return EMPTY;

    return keyword;
}

const String &parseKeyword(Environment &e) {
    const String &keyword = peekKeyword(e);
    e.current() += keyword.length();
    return keyword;
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
