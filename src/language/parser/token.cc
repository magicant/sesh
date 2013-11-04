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

namespace {

/** Remove line continuations if any and return the current character. */
CharTraits::int_type charAt(SourceBuffer::Difference n, Environment &e) {
    auto i = e.current() + n;
    while (e.removeLineContinuation(i)) { }
    return dereference(e, i);
}

} // namespace

#define IL(x) (CharTraits::to_int_type(L(x)))

String peekSymbol(Environment &e) {
    SourceBuffer::Size length;

    switch (charAt(0, e)) {
    case IL('&'):
        switch (charAt(1, e)) {
        case IL('&'):  length = 2;  break;
        default:       length = 1;  break;
        }
        break;
    case IL('|'):
        switch (charAt(1, e)) {
        case IL('|'):  length = 2;  break;
        default:       length = 1;  break;
        }
        break;
    case IL(';'):
        switch (charAt(1, e)) {
        case IL(';'):  length = 2;  break;
        default:       length = 1;  break;
        }
        break;
    case IL('<'):
        switch (charAt(1, e)) {
        case IL('<'):
            switch (charAt(2, e)) {
            case IL('-'):  length = 3;  break;
            default:       length = 2;  break;
            }
            break;
        case IL('>'):
        case IL('&'):
        case IL('|'):
            length = 2;
            break;
        default:
            length = 1;
            break;
        }
        break;
    case IL('>'):
        switch (charAt(1, e)) {
        case IL('>'):
        case IL('&'):
        case IL('|'):
            length = 2;
            break;
        default:
            length = 1;
            break;
        }
        break;
    case IL('('):
        length = 1;
        break;
    case IL(')'):
        length = 1;
        break;
    default:
        length = 0;
        break;
    }

    Iterator begin = e.current();
    Iterator end = begin + length;
    return toString(begin, end);
}

String parseSymbol(Environment &e) {
    String symbol = peekSymbol(e);
    e.current() += symbol.length();
    return symbol;
}

namespace {

/**
 * Checks if there is a specified string at the current position. If there is,
 * the length of the string is returned. Otherwise, zero is returned.
 *
 * Line continuations are removed while parsing. The current position of the
 * environment is not changed.
 */
String::size_type matchStringAndGetLength(Environment &e, const String &s) {
    for (String::size_type i = 0; i < s.length(); i++) {
        if (charAt(i, e) != CharTraits::to_int_type(s[i]))
            return 0;
    }
    return s.length();
}

} // namespace

String peekKeyword(Environment &e) {
    static const String CASE = L("case");
    static const String FUNCTION = L("function");
    static const String SELECT = L("select");
    static const String THEN = L("then");
    static const String UNTIL = L("until");
    static const String WHILE = L("while");

    SourceBuffer::Size length;

    // Keywords: ! [[ ]] { } case do done elif else esac fi for function if in
    // select then until while
    switch (auto c = charAt(0, e)) {
    case IL('!'):
    case IL('{'):
    case IL('}'):
        length = 1;
        break;
    case IL('['):
    case IL(']'):
        length = (charAt(1, e) == c) ? 2 : 0; // [[ ]]
        break;
    case IL('c'):
        length = matchStringAndGetLength(e, CASE);
        break;
    case IL('d'):
        if (charAt(1, e) != IL('o')) {
            length = 0;
            break;
        }
        if (charAt(2, e) == IL('n') && charAt(3, e) == IL('e'))
            length = 4; // done
        else
            length = 2; // do
        break;
    case IL('e'):
        switch (charAt(1, e)) {
        case IL('l'):
            if ((charAt(2, e) == IL('i') && charAt(3, e) == IL('f')) ||
                    (charAt(2, e) == IL('s') && charAt(3, e) == IL('e')))
                length = 4; // elif else
            else
                length = 0;
            break;
        case IL('s'):
            if (charAt(2, e) == IL('a') && charAt(3, e) == IL('c'))
                length = 4; // esac
            else
                length = 0;
            break;
        default:
            length = 0;
            break;
        }
        break;
    case IL('f'):
        switch (charAt(1, e)) {
        case IL('i'):
            length = 2; // fi
            break;
        case IL('o'):
            length = (charAt(2, e) == IL('r')) ? 3 : 0; // for
            break;
        case IL('u'):
            length = matchStringAndGetLength(e, FUNCTION);
            break;
        default:
            length = 0;
            break;
        }
        break;
    case IL('i'):
        switch (charAt(1, e)) {
        case IL('f'):
        case IL('n'):
            length = 2; // if in
            break;
        default:
            length = 0;
            break;
        }
        break;
    case IL('s'):
        length = matchStringAndGetLength(e, SELECT);
        break;
    case IL('t'):
        length = matchStringAndGetLength(e, THEN);
        break;
    case IL('u'):
        length = matchStringAndGetLength(e, UNTIL);
        break;
    case IL('w'):
        length = matchStringAndGetLength(e, WHILE);
        break;
    default:
        length = 0;
        break;
    }

    // A keyword must be followed by EOF or a token delimiter.
    auto charAfterKeyword = charAt(length, e);
    if (!CharTraits::eq_int_type(charAfterKeyword, CharTraits::eof()) &&
            !isTokenDelimiter(e, CharTraits::to_char_type(charAfterKeyword)))
        length = 0;

    Iterator begin = e.current();
    Iterator end = begin + length;
    return toString(begin, end);
}

String parseKeyword(Environment &e) {
    String keyword = peekKeyword(e);
    e.current() += keyword.length();
    return keyword;
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
