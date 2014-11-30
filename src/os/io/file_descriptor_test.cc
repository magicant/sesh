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

#include <utility>
#include "catch.hpp"
#include "os/io/file_descriptor.hh"

namespace {

using sesh::os::io::file_descriptor;

TEST_CASE("File descriptor, construction, invalid") {
    file_descriptor fd;
    CHECK_FALSE(fd.is_valid());
}

TEST_CASE("File descriptor, construction and clear") {
    file_descriptor fd(42);
    CHECK(fd.value() == 42);
    CHECK(fd.is_valid());
    fd.clear();
    CHECK_FALSE(fd.is_valid());
}

TEST_CASE("File descriptor, move construction") {
    file_descriptor fd1(0);
    file_descriptor fd2 = std::move(fd1);
    CHECK_FALSE(fd1.is_valid());
    CHECK(fd2.is_valid());
    fd2.clear();
}

TEST_CASE("File descriptor, swap, member") {
    file_descriptor fd1(3), fd2(9);
    fd1.swap(fd2);
    CHECK(fd1.value() == 9);
    CHECK(fd2.value() == 3);
    fd1.clear();
    fd2.clear();
}

TEST_CASE("File descriptor, move assignment") {
    file_descriptor fd1;
    file_descriptor fd2(0);
    fd1 = std::move(fd2);
    CHECK(fd1.is_valid());
    CHECK_FALSE(fd2.is_valid());
    fd1.clear();
}

TEST_CASE("File descriptor, swap, non-member") {
    file_descriptor fd1(3), fd2(9);
    swap(fd1, fd2);
    CHECK(fd1.value() == 9);
    CHECK(fd2.value() == 3);
    fd1.clear();
    fd2.clear();
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
