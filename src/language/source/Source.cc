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

#include "Source.hh"
#include <stdexcept>
#include <utility>

namespace sesh {
namespace language {
namespace source {

auto Source::positionAfterAlternate() const noexcept -> Size {
    return begin() + alternate().length();
}

auto Source::lengthDifference() const noexcept -> Difference {
    return static_cast<Difference>(alternate().length()) -
            static_cast<Difference>(end() - begin());
}

Source::Source(Pointer &&original, Size begin, Size end, String &&alternate) :
        mOriginal(std::move(original)),
        mBegin(begin),
        mEnd(end),
        mAlternate(std::move(alternate)) {
    Size originalLength = (mOriginal == nullptr) ? 0 : mOriginal->length();
    Difference ld = lengthDifference();
    Size thisLength = originalLength + ld;

    if (mBegin > mEnd || mEnd > originalLength)
        throw std::out_of_range("invalid alteration range");
    if (ld > 0 && thisLength < originalLength)
        throw std::overflow_error("too long source");
}

auto Source::length() const noexcept -> Size {
    if (original() == nullptr)
        return alternate().length();

    return original()->length() + lengthDifference();
}

auto Source::at(Size position) const -> ConstReference {
    if (original() == nullptr)
        return alternate().at(position);

    if (position < begin())
        return original()->at(position);

    Size positionFromBegin = position - begin();
    if (positionFromBegin < alternate().length())
        return alternate().at(positionFromBegin);

    Size positionFromEnd = positionFromBegin - alternate().length() + end();
    if (positionFromEnd < end())
        positionFromEnd = String::npos; // recover overflow
    return original()->at(positionFromEnd);
}

auto Source::operator[](Size position) const -> ConstReference {
    if (original() == nullptr)
        return alternate()[position];

    if (position < begin())
        return (*original())[position];

    Size positionFromBegin = position - begin();
    if (positionFromBegin < alternate().length())
        return alternate()[positionFromBegin];

    return (*original())[positionFromBegin - alternate().length() + end()];
}

auto Source::lineBeginInAlternate(Size position) const noexcept -> Size {
    if (position == 0)
        return 0;

    Size newlinePosition = alternate().rfind(NEWLINE, position - 1);
    if (newlinePosition == String::npos)
        return 0;
    return newlinePosition + 1;
}

auto Source::lineEndInAlternate(Size position) const noexcept -> Size {
    Size newlinePosition = alternate().find(NEWLINE, position);
    if (newlinePosition == String::npos)
        return String::npos;
    return newlinePosition + 1;
}

auto Source::lineBegin(Size position) const noexcept -> Size {
    if (original() != nullptr && position > positionAfterAlternate()) {
        Difference ld = lengthDifference();
        Size lineBeginAfterAlternate = original()->lineBegin(position - ld);
        if (lineBeginAfterAlternate > end())
            return lineBeginAfterAlternate + ld;
        position = positionAfterAlternate();
    }

    if (position > begin()) {
        Size positionInAlternate = position - begin();
        Size altLineBegin = lineBeginInAlternate(positionInAlternate);
        if (altLineBegin > 0)
            return altLineBegin + begin();
        position = begin();
    }

    if (original() == nullptr)
        return 0;
    return original()->lineBegin(position);
}

auto Source::lineEnd(Size position) const noexcept -> Size {
    if (position < begin()) {
        Size lineEndBeforeBegin = original()->lineEnd(position);
        if (lineEndBeforeBegin < begin())
            return lineEndBeforeBegin;
        if (lineEndBeforeBegin == begin())
            if ((*original())[lineEndBeforeBegin - 1] == NEWLINE)
                return lineEndBeforeBegin;
        position = begin();
    }

    if (position < positionAfterAlternate()) {
        Size positionInAlternate = position - begin();
        Size altLineEnd = lineEndInAlternate(positionInAlternate);
        if (altLineEnd != String::npos)
            return altLineEnd + begin();
        position = positionAfterAlternate();
    }

    if (original() == nullptr)
        return position;

    Difference ld = lengthDifference();
    return original()->lineEnd(position - ld) + ld;
}

SourceLocation Source::location(Size position) const {
    if (position < begin())
        return original()->location(position);
    if (position < positionAfterAlternate())
        return locationInAlternate(position - begin());
    return original()->location(position - lengthDifference());
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
