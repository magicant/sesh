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

#include <string>
#include "catch.hpp"
#include "common/visitor.hh"

namespace {

using sesh::common::visitable;
using sesh::common::visitable_value;
using sesh::common::visitor;

std::string to_string(int i) {
    return std::to_string(i);
}

std::string to_string(char c) {
    return std::string(1, c);
}

class string_converter {

private:

    std::string &s;

public:

    explicit string_converter(std::string &s) noexcept : s(s) { }

    template<typename... A>
    void operator()(A &&... a) const {
        s = to_string(std::forward<A>(a)...);
    }

}; // class string_converter

using V = visitor<int, char>;

std::string to_string(const visitable<V> &v) {
    std::string s;
    visit(v, string_converter(s));
    return s;
}

TEST_CASE("Visit int") {
    const visitable_value<V, int> v(3);
    CHECK(to_string(v) == "3");
}

TEST_CASE("Visit char") {
    const visitable_value<V, char> v('!');
    CHECK(to_string(v) == "!");
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
