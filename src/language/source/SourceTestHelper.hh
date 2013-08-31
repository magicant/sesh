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

#ifndef INCLUDED_language_source_SourceTestHelper_hh
#define INCLUDED_language_source_SourceTestHelper_hh

#include <vector>
#include "catch.hpp"
#include "language/source/Source.hh"

namespace sesh {
namespace language {
namespace source {

class SourceStub : public Source {
    using Source::Source;
    SourceLocation locationInAlternate(Size) const override {
        throw "unexpected location";
    }
};

void checkSourceString(const Source &source, const Source::String &string) {
    for (Source::Size i = 0; i < string.length(); ++i) {
        CHECK(source.at(i) == string.at(i));
        CHECK(source[i] == string[i]);
    }
    CHECK_THROWS_AS(source.at(string.length()), std::out_of_range);
    CHECK(source[string.length()] == Source::Char());
}

void checkSourceLineBegin(
        const Source &source, const std::vector<Source::Size> &linePositions) {
    Source::Size position = 0;
    Source::Size lineBeginPosition = 0;

    for (auto i = linePositions.cbegin(); i != linePositions.cend(); ++i) {
        Source::Size lineEndPosition = *i;
        for (; position < lineEndPosition; ++position) {
            INFO("position=" << position);
            CHECK(source.lineBegin(position) == lineBeginPosition);
        }
        lineBeginPosition = lineEndPosition;
    }

    Source::Size length = source.length();
    for (; position <= length; ++position) {
        INFO("position=" << position);
        CHECK(source.lineBegin(position) == lineBeginPosition);
    }
}

void checkSourceLineEnd(
        const Source &source, const std::vector<Source::Size> &linePositions) {
    Source::Size position = 0;

    for (auto i = linePositions.cbegin(); i != linePositions.cend(); ++i) {
        Source::Size lineEndPosition = *i;
        for (; position < lineEndPosition; ++position) {
            INFO("position=" << position);
            CHECK(source.lineEnd(position) == lineEndPosition);
        }
    }
}

void checkSourceLocation(
        const Source &source,
        Source::Size position,
        const Source::String &name,
        Source::Size line,
        Source::Size column) {
    SourceLocation sl = source.location(position);
    CHECK(sl.name() == name);
    CHECK(sl.line() == line);
    CHECK(sl.column() == column);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_SourceTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
