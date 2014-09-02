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

#ifndef INCLUDED_language_source_source_test_helper_hh
#define INCLUDED_language_source_source_test_helper_hh

#include "buildconfig.h"

#include <vector>
#include "catch.hpp"
#include "language/source/OriginTestHelper.hh"
#include "language/source/source.hh"

namespace sesh {
namespace language {
namespace source {

class source_stub : public source {
    using source::source;
    Location location_in_alternate(size_type) const override {
        throw "unexpected location";
    }
};

void check_source_string(
        const source &src, const source::string_type &string) {
    for (source::size_type i = 0; i < string.length(); ++i) {
        CHECK(src.at(i) == string.at(i));
        CHECK(src[i] == string[i]);
    }
    CHECK_THROWS_AS(src.at(string.length()), std::out_of_range);
    CHECK(src[string.length()] == source::value_type());
}

void check_source_line_begin(
        const source &source,
        const std::vector<source::size_type> &line_positions) {
    source::size_type position = 0;
    source::size_type line_begin_position = 0;

    for (auto i = line_positions.cbegin(); i != line_positions.cend(); ++i) {
        source::size_type line_end_position = *i;
        for (; position < line_end_position; ++position) {
            INFO("position=" << position);
            CHECK(source.line_begin(position) == line_begin_position);
        }
        line_begin_position = line_end_position;
    }

    source::size_type length = source.length();
    for (; position <= length; ++position) {
        INFO("position=" << position);
        CHECK(source.line_begin(position) == line_begin_position);
    }
}

void check_source_line_end(
        const source &source,
        const std::vector<source::size_type> &line_positions) {
    source::size_type position = 0;

    for (auto i = line_positions.cbegin(); i != line_positions.cend(); ++i) {
        source::size_type line_end_position = *i;
        for (; position < line_end_position; ++position) {
            INFO("position=" << position);
            CHECK(source.line_end(position) == line_end_position);
        }
    }
}

void check_source_location(
        const source &source,
        source::size_type position,
        source::size_type line,
        source::size_type column) {
    Location l = source.location(position);
    CHECK_NOTHROW((void) dynamic_cast<const OriginStub &>(l.origin()));
    CHECK(l.line() == line);
    CHECK(l.column() == column);
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_source_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
