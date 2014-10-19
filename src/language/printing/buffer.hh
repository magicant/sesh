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

#ifndef INCLUDED_language_printing_buffer_hh
#define INCLUDED_language_printing_buffer_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "common/xsstream.hh"
#include "common/xstring.hh"

namespace sesh {
namespace language {
namespace printing {

/**
 * A buffer is a collection of three output string streams used while
 * converting a syntax object to a string. The three streams are called the
 * main buffer, the delayed line buffer, and the delayed character buffer.
 *
 * The conversion cannot be done by simply using one output stream because of
 * complexity of the shell syntax.
 *
 * Words are separated by a space in the result string, but no space is
 * required after the last word. To prevent insertion of the unnecessary space,
 * the space is first inserted to the delayed character buffer. If the space is
 * later found to be unnecessary, the buffer contents can be deleted. If left
 * undeleted, the buffer contents are sent to the main buffer just before other
 * characters are inserted to the main buffer.
 *
 * The contents of here-documents should be inserted between command lines
 * (rather than in the middle of lines), so here-documents are first inserted
 * to the delayed line buffer. When a newline is appended to the main buffer,
 * the contents of the delayed line buffer is sent to the main buffer.
 *
 * A buffer also remembers an indent level and inserts indentations of the
 * desired width.
 *
 * A buffer has a line mode, either single line or multi-line. The mode
 * selection affects insertion of newlines, indentations, and delayed buffer
 * contents.
 */
class buffer {

public:

    enum class line_mode_type {
        single_line,
        multi_line,
    };

    using size_type = common::xstring::size_type;

private:

    common::xostringstream m_main_buffer;
    common::xostringstream m_delayed_characters;
    common::xostringstream m_delayed_lines;
    line_mode_type m_line_mode;
    size_type m_indent_level;

public:

    explicit buffer(line_mode_type);

    line_mode_type line_mode() const noexcept {
        return m_line_mode;
    }

    size_type &indent_level() noexcept { return m_indent_level; }
    size_type indent_level() const noexcept { return m_indent_level; }

    /**
     * Appends the argument to the main buffer after committing the delayed
     * characters.
     */
    template<typename T>
    void append_main(T &&v) {
        commit_delayed_characters();
        m_main_buffer << std::forward<T>(v);
    }

    /** Appends the argument to the delayed character buffer. */
    template<typename T>
    void append_delayed_characters(T &&v) {
        m_delayed_characters << std::forward<T>(v);
    }

    /** Appends the argument to the delayed line buffer. */
    template<typename T>
    void append_delayed_lines(T &&v) {
        m_delayed_lines << std::forward<T>(v);
    }

    void clear_delayed_characters();
    void commit_delayed_characters();

    /**
     * If the line mode is multi_line, a newline and the contents of the
     * delayed line buffer are appended to the main buffer and the delayed
     * character buffer is cleared.
     *
     * If the line mode is single_line, the main and delayed character buffer
     * are not affected.
     *
     * In either case, the delayed line buffer is cleared.
     */
    void break_line();

    /**
     * Insert spaces to the delayed character buffer if the line mode is
     * multi_line. The number of inserted spaces is determined by the current
     * indent level.
     */
    void indent();

    /** Returns the contents of the main buffer. */
    common::xstring to_string() const;

}; // class buffer

/**
 * The buffer printer is a wrapper of a reference to a buffer and is callable
 * with any one argument, which is forwarded to the print function with the
 * buffer.
 */
class buffer_printer {

private:

    buffer &m_buffer;

public:

    explicit buffer_printer(buffer &b) noexcept : m_buffer(b) { }

    template<typename T>
    void operator()(T &&v) const {
        print(std::forward<T>(v), m_buffer);
    }

}; // class buffer_printer

} // namespace printing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_printing_buffer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
