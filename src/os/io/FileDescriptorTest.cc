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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <utility>
#include "os/io/FileDescriptor.hh"

namespace {

using sesh::os::io::FileDescriptor;

TEST_CASE("File descriptor, construction, invalid") {
    FileDescriptor fd;
    CHECK_FALSE(fd.isValid());
}

TEST_CASE("File descriptor, construction and clear") {
    FileDescriptor fd(42);
    CHECK(fd.value() == 42);
    CHECK(fd.isValid());
    fd.clear();
    CHECK_FALSE(fd.isValid());
}

TEST_CASE("File descriptor, move construction") {
    FileDescriptor fd1(0);
    FileDescriptor fd2 = std::move(fd1);
    CHECK_FALSE(fd1.isValid());
    CHECK(fd2.isValid());
    fd2.clear();
}

TEST_CASE("File descriptor, swap, member") {
    FileDescriptor fd1(3), fd2(9);
    fd1.swap(fd2);
    CHECK(fd1.value() == 9);
    CHECK(fd2.value() == 3);
    fd1.clear();
    fd2.clear();
}

TEST_CASE("File descriptor, move assignment") {
    FileDescriptor fd1;
    FileDescriptor fd2(0);
    fd1 = std::move(fd2);
    CHECK(fd1.isValid());
    CHECK_FALSE(fd2.isValid());
    fd1.clear();
}

TEST_CASE("File descriptor, swap, non-member") {
    FileDescriptor fd1(3), fd2(9);
    swap(fd1, fd2);
    CHECK(fd1.value() == 9);
    CHECK(fd2.value() == 3);
    fd1.clear();
    fd2.clear();
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
