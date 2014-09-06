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
#include "printer.hh"

#include "common/xchar.hh"
#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

using common::xstring;

} // namespace

printer::printer(line_mode_type line_mode) :
        m_line_mode(line_mode),
        m_main_buffer(),
        m_delayed_characters(),
        m_delayed_lines(),
        m_indent_level() {
}

xstring printer::to_string() const {
    return m_main_buffer.str();
}

void printer::clear_delayed_characters() {
    m_delayed_characters.str(xstring());
}

void printer::commit_delayed_characters(){
    m_main_buffer << m_delayed_characters.str();
    clear_delayed_characters();
}

/**
 * If the line mode is multi_line, a newline and the contents of the delayed
 * line buffer are appended to the main buffer and the delayed character buffer
 * is cleared.
 *
 * If the line mode is single_line, the contents of the delayed character
 * buffer is set to a single space. The delayed line buffer is ignored.
 *
 * In either case, the delayed line buffer is cleared.
 */
void printer::break_line() {
    switch (m_line_mode) {
    case line_mode_type::single_line:
        m_delayed_characters.str(L(" "));
        break;
    case line_mode_type::multi_line:
        clear_delayed_characters();
        m_main_buffer << L('\n') << m_delayed_lines.str();
        break;
    }

    m_delayed_lines.str(xstring());
}

/**
 * Inserts a series of spaces. The number of spaces is determined from the
 * current indent level. The delayed character buffer is ignored. This function
 * does nothing if the line mode is not multi_line.
 */
void printer::print_indent() {
    switch (m_line_mode) {
    case line_mode_type::single_line:
        return;
    case line_mode_type::multi_line:
        m_main_buffer << xstring(4 * m_indent_level, L(' '));
        return;
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
