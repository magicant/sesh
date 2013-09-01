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

#include "Printer.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

using String = common::String;

} // namespace

Printer::Printer(LineMode lineMode) :
        mLineMode(lineMode),
        mMainBuffer(),
        mDelayedCharacters(),
        mDelayedLines(),
        mIndentLevel() {
}

String Printer::toString() const {
    return mMainBuffer.str();
}

void Printer::clearDelayedCharacters() {
    mDelayedCharacters.str(String());
}

void Printer::commitDelayedCharacters(){
    mMainBuffer << mDelayedCharacters.str();
    clearDelayedCharacters();
}

/**
 * If the line mode is MULTI_LINE, a newline and the contents of the delayed
 * line buffer are appended to the main buffer and the delayed character buffer
 * is cleared.
 *
 * If the line mode is SINGLE_LINE, the contents of the delayed character
 * buffer is set to a single space. The delayed line buffer is ignored.
 *
 * In either case, the delayed line buffer is cleared.
 */
void Printer::breakLine() {
    switch (mLineMode) {
    case LineMode::SINGLE_LINE:
        mDelayedCharacters.str(L" ");
        break;
    case LineMode::MULTI_LINE:
        clearDelayedCharacters();
        mMainBuffer << L'\n' << mDelayedLines.str();
        break;
    }

    mDelayedLines.str(String());
}

/**
 * Inserts a series of spaces. The number of spaces is determined from the
 * current indent level. The delayed character buffer is ignored. This function
 * does nothing if the line mode is not MULTI_LINE.
 */
void Printer::printIndent() {
    switch (mLineMode) {
    case LineMode::SINGLE_LINE:
        return;
    case LineMode::MULTI_LINE:
        mMainBuffer << String(4 * mIndentLevel, L' ');
        return;
    }
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
