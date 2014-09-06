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

#ifndef INCLUDED_language_syntax_printer_hh
#define INCLUDED_language_syntax_printer_hh

#include "buildconfig.h"

#include <cstddef>
#include <type_traits>
#include <utility>
#include "common/xostream.hh"
#include "common/xsstream.hh"
#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace syntax {

class printable;

/**
 * Printer is an intermediate object that is used while converting a command
 * into a string. Printer is essentially a wrapper of output string stream, but
 * it has two auxiliary buffers called the delayed character buffer and the
 * delayed line buffer in addition to the main buffer.
 *
 * Words may be separated by spaces when printed, but no space is required
 * after the last word. To prevent insertion of the unnecessary space, the
 * space is first inserted to the delayed character buffer. If the space is
 * later found to be unnecessary, the buffer contents can be deleted. If left
 * undeleted, the buffer contents are sent to the main buffer just before a
 * normal character is inserted to the main buffer.
 *
 * The contents of here-documents should be inserted between command lines
 * (rather than in the middle of lines), so here-documents are first inserted
 * to the delayed line buffer. When a newline is appended to the main buffer,
 * the contents of the delayed line buffer is sent to the main buffer.
 *
 * A printer can remember an indent level and insert indentations of the
 * desired width.
 *
 * A printer can have a line mode, either single line or multi-line. The mode
 * selection affects insertion of newlines, indentations, and delayed buffer
 * contents.
 */
class printer {

public:

    enum class line_mode_type {
        single_line,
        multi_line,
    };

private:

    const line_mode_type m_line_mode;
    common::xostringstream m_main_buffer;
    common::xostringstream m_delayed_characters;
    common::xostringstream m_delayed_lines;
    std::size_t m_indent_level;

public:

    explicit printer(line_mode_type);

    printer(const printer &) = delete;
    printer(printer &&) = default;
    printer &operator=(const printer &) = delete;
    printer &operator=(printer &&) = default;
    ~printer() = default;

    line_mode_type line_mode() const noexcept {
        return m_line_mode;
    }

    common::xstring to_string() const;

    common::xostream &delayed_characters() noexcept {
        return m_delayed_characters;
    }

    common::xostream &delayed_lines() noexcept {
        return m_delayed_lines;
    }

    void clear_delayed_characters();
    void commit_delayed_characters();

    /* If T is printable, another operator overload defined in "printable.hh"
     * is used. */
    template<
            typename T,
            typename = typename std::enable_if<!std::is_base_of<
                    printable,
                    typename std::decay<T>::type
                    >::value>::type>
    printer &operator<<(T &&v) {
        commit_delayed_characters();
        m_main_buffer << std::forward<T>(v);
        return *this;
    }

    void break_line();

    std::size_t &indent_level() noexcept { return m_indent_level; }
    std::size_t indent_level() const noexcept { return m_indent_level; }

    void print_indent();

public:

    class indent_guard {

    private:

        printer &m_printer;
        const std::size_t m_old_indent_level;

    public:

        explicit indent_guard(
                printer &p, std::size_t indent_level_increment = 1) noexcept :
                m_printer(p), m_old_indent_level(p.indent_level()) {
            m_printer.indent_level() += indent_level_increment;
        }

        indent_guard(const indent_guard &) = delete;
        indent_guard(indent_guard &&) = delete;
        indent_guard &operator=(const indent_guard &) = delete;
        indent_guard &operator=(indent_guard &&) = delete;

        ~indent_guard() noexcept {
            m_printer.indent_level() = m_old_indent_level;
        }

    }; // class indent_guard

}; // class printer

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_printer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
