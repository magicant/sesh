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
#include "buffer.hh"

#include <limits>
#include "common/xchar.hh"
#include "common/xstring.hh"

namespace {

using sesh::common::xstring;

} // namespace

namespace sesh {
namespace language {
namespace printing {

buffer::buffer(line_mode_type mode) :
        m_main_buffer(),
        m_delayed_characters(),
        m_delayed_lines(),
        m_line_mode(mode),
        m_indent_level() { }

void buffer::clear_delayed_characters() {
    m_delayed_characters.str({});
}

void buffer::commit_delayed_characters() {
    m_main_buffer << m_delayed_characters.str();
    clear_delayed_characters();
}

void buffer::break_line() {
    switch (line_mode()) {
    case line_mode_type::single_line:
        break;
    case line_mode_type::multi_line:
        clear_delayed_characters();
        m_main_buffer << L('\n') << m_delayed_lines.str();
        break;
    }
    m_delayed_lines.str({});
}

void buffer::indent() {
    switch (line_mode()) {
    case line_mode_type::single_line:
        return;
    case line_mode_type::multi_line:
        constexpr size_type width = 4;
        size_type count = width * indent_level();
        if (count / width != indent_level())
            count = std::numeric_limits<size_type>::max();
        append_delayed_characters(xstring(count, L(' ')));
        return;
    }
}

xstring buffer::to_string() const {
    return m_main_buffer.str();
}

} // namespace printing
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
