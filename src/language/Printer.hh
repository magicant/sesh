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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_Printer_hh
#define INCLUDED_language_Printer_hh

#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace sesh {
namespace language {

class Printable;

/**
 * Printer is an intermediate object that is used while converting a command
 * into a string. Printer is essentially a wrapper of wostringstream, but it
 * has two auxiliary buffers called the delayed character buffer and the
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
class Printer {

public:

    enum class LineMode {
        SINGLE_LINE,
        MULTI_LINE,
    };

private:

    const LineMode mLineMode;
    std::wostringstream mMainBuffer;
    std::wostringstream mDelayedCharacters;
    std::wostringstream mDelayedLines;
    std::size_t mIndentLevel;

public:

    explicit Printer(LineMode);

    Printer(const Printer &) = delete;
    Printer(Printer &&) = default;
    Printer &operator=(const Printer &) = delete;
    Printer &operator=(Printer &&) = default;
    ~Printer() = default;

    LineMode lineMode() const noexcept {
        return mLineMode;
    }

    std::wstring toWstring() const;

    std::wostream &delayedCharacters() noexcept {
        return mDelayedCharacters;
    }

    std::wostream &delayedLines() noexcept {
        return mDelayedLines;
    }

    void clearDelayedCharacters();
    void commitDelayedCharacters();

    /* If T is Printable, another operator overload defined in "Printable.hh"
     * is used. */
    template<
            typename T,
            typename = typename std::enable_if<!std::is_base_of<
                    Printable,
                    typename std::decay<T>::type
                    >::value>::type>
    Printer &operator<<(T &&v) {
        commitDelayedCharacters();
        mMainBuffer << std::forward<T>(v);
        return *this;
    }

    void breakLine();

    std::size_t &indentLevel() noexcept { return mIndentLevel; }
    std::size_t indentLevel() const noexcept { return mIndentLevel; }

    void printIndent();

public:

    class IndentGuard {

    private:

        Printer &mPrinter;
        const std::size_t mOldIndentLevel;

    public:

        explicit IndentGuard(Printer &p, std::size_t indentLevelIncrement = 1)
                noexcept :
                mPrinter(p), mOldIndentLevel(p.indentLevel()) {
            mPrinter.indentLevel() += indentLevelIncrement;
        }

        IndentGuard(const IndentGuard &) = delete;
        IndentGuard(IndentGuard &&) = delete;
        IndentGuard &operator=(const IndentGuard &) = delete;
        IndentGuard &operator=(IndentGuard &&) = delete;

        ~IndentGuard() noexcept {
            mPrinter.indentLevel() = mOldIndentLevel;
        }

    }; // class IndentGuard

}; // class Printer

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_Printer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
