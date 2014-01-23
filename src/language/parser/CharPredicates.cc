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
#include "CharPredicates.hh"

#include <locale>
#include "common/Char.hh"
#include "common/String.hh"
#include "config.h"
#include "language/parser/Environment.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::common::contains;

namespace sesh {
namespace language {
namespace parser {

bool isTokenDelimiter(const Environment &e, Char c) {
    static const String delimiters = L(" \t\n;&|<>()");
    return contains(delimiters, c) ||
#if HAVE_STD__ISBLANK
            std::isblank(c, e.locale())
#else
            ((void) e.locale(), false)
#endif // #if HAVE_STD__ISBLANK
            ;
}

bool isRawStringChar(const Environment &e, Char c) {
    static const String specialChars = L("\\\"\'$`");
    return !contains(specialChars, c) && !isTokenDelimiter(e, c);
}

bool isVariableNameChar(const Environment &e, Char c) {
    return c == L('_') || std::isalnum(c, e.locale());
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */