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

#ifndef INCLUDED_language_syntax_printer_test_helper_hh
#define INCLUDED_language_syntax_printer_test_helper_hh

#include "buildconfig.h"

#include <functional>
#include "language/syntax/printer.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

inline void test(
        std::function<void(printer &)> f, printer::line_mode_type lm) {
    printer p(lm);
    f(p);
}

} // namespace

inline void for_each_line_mode(std::function<void(printer &)> f) {
    test(f, printer::line_mode_type::single_line);
    test(f, printer::line_mode_type::multi_line);
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_printer_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
