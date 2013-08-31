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

#ifndef INCLUDED_language_syntax_PrinterTestHelper_hh
#define INCLUDED_language_syntax_PrinterTestHelper_hh

#include <functional>
#include "language/syntax/Printer.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

inline void test(std::function<void(Printer &)> f, Printer::LineMode lm) {
    Printer p(lm);
    f(p);
}

} // namespace

inline void forEachLineMode(std::function<void(Printer &)> f) {
    test(f, Printer::LineMode::SINGLE_LINE);
    test(f, Printer::LineMode::MULTI_LINE);
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_PrinterTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
