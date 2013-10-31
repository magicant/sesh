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

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
