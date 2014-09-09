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

#include <memory>
#include <system_error>
#include "common/variant.hh"
#include "os/io/file_description_access_mode.hh"
#include "os/io/file_description_api.hh"
#include "os/io/FileDescriptionAttribute.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptor.hh"

namespace {

using sesh::common::variant;
using sesh::os::io::file_description_access_mode;
using sesh::os::io::file_description_api;
using sesh::os::io::FileDescriptionAttribute;
using sesh::os::io::FileDescriptionStatus;
using sesh::os::io::FileDescriptor;
using sesh::os::io::NonBlockingFileDescriptor;

namespace closed_fd {

class FileDescriptionApiMock : public file_description_api {

public:

    constexpr static FileDescriptor::Value value() noexcept { return 3; }

    variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
    get_file_description_status(const FileDescriptor &fd) const override {
        CHECK(fd.isValid());
        CHECK(fd.value() == value());
        return std::make_error_code(std::errc::bad_file_descriptor);
    }

    std::error_code set_file_description_status(
            const FileDescriptor &, const FileDescriptionStatus &) const
            override {
        throw "unexpected set_file_description_status";
    }

}; // class FileDescriptionApiMock

TEST_CASE("Non-blocking file descriptor: construction with closed FD") {
    const auto api = FileDescriptionApiMock();
    auto nbfd = NonBlockingFileDescriptor(api, FileDescriptor(api.value()));
    CHECK(nbfd.isValid());
    CHECK(nbfd.value() == api.value());
    nbfd.release().clear();
}

TEST_CASE("Non-blocking file descriptor: releasing closed FD") {
    const auto api = FileDescriptionApiMock();
    auto nbfd = NonBlockingFileDescriptor(api, FileDescriptor(api.value()));
    FileDescriptor fd = nbfd.release();
    CHECK_FALSE(nbfd.isValid());
    CHECK(fd.isValid());
    CHECK(fd.value() == api.value());
    fd.clear();
}

} // namespace closed_fd

namespace open_fd {

class FileDescriptionStatusMock : public FileDescriptionStatus {

public:

    bool isNonBlocking;

    explicit FileDescriptionStatusMock(bool isNonBlocking) noexcept :
            isNonBlocking(isNonBlocking) { }

    bool test(FileDescriptionAttribute a) const noexcept override {
        CHECK(a == FileDescriptionAttribute::NON_BLOCKING);
        return isNonBlocking;
    }

    FileDescriptionStatus &set(FileDescriptionAttribute a, bool value = true)
            noexcept {
        CHECK(a == FileDescriptionAttribute::NON_BLOCKING);
        isNonBlocking = value;
        return *this;
    }

    std::unique_ptr<FileDescriptionStatus> clone() const override {
        return std::unique_ptr<FileDescriptionStatus>(new auto(*this));
    }

    file_description_access_mode accessMode() const noexcept override {
        throw "unexpected accessMode";
    }
    FileDescriptionStatus &resetAttributes() noexcept override {
        throw "unexpected resetAttributes";
    }

}; // class FileDescriptionStatusMock

class FileDescriptionApiMock : public file_description_api {

public:

    constexpr static FileDescriptor::Value value() noexcept { return 5; }

    mutable bool isNonBlocking;

    explicit FileDescriptionApiMock(bool isNonBlocking) noexcept :
            isNonBlocking(isNonBlocking) { }

    variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
    get_file_description_status(const FileDescriptor &fd) const override {
        CHECK(fd.isValid());
        CHECK(fd.value() == value());
        return std::unique_ptr<FileDescriptionStatus>(
                new FileDescriptionStatusMock(isNonBlocking));
    }

    std::error_code set_file_description_status(
            const FileDescriptor &fd, const FileDescriptionStatus &status)
            const override {
        CHECK(fd.isValid());
        CHECK(fd.value() == value());
        const FileDescriptionStatusMock &statusMock =
                dynamic_cast<const FileDescriptionStatusMock &>(status);
        isNonBlocking = statusMock.isNonBlocking;
        return std::error_code();
    }

}; // class FileDescriptionApiMock

void testConstruction(bool isInitiallyNonBlocking) {
    const auto api = FileDescriptionApiMock(isInitiallyNonBlocking);
    auto nbfd = NonBlockingFileDescriptor(api, FileDescriptor(api.value()));
    CHECK(nbfd.isValid());
    CHECK(nbfd.value() == api.value());
    CHECK(api.isNonBlocking);
    nbfd.release().clear();
}

TEST_CASE("Non-blocking file descriptor: construction with blocking FD") {
    testConstruction(false);
}

TEST_CASE("Non-blocking file descriptor: construction with non-blocking FD") {
    testConstruction(true);
}

void testRelease(bool isInitiallyNonBlocking) {
    const auto api = FileDescriptionApiMock(isInitiallyNonBlocking);
    auto nbfd = NonBlockingFileDescriptor(api, FileDescriptor(api.value()));
    FileDescriptor fd = nbfd.release();
    CHECK_FALSE(nbfd.isValid());
    CHECK(fd.isValid());
    CHECK(fd.value() == api.value());
    CHECK(api.isNonBlocking == isInitiallyNonBlocking);
    fd.clear();
}

TEST_CASE("Non-blocking file descriptor: releasing originally blocking FD") {
    testRelease(false);
}

TEST_CASE(
        "Non-blocking file descriptor: releasing originally non-blocking FD") {
    testRelease(true);
}

} // namespace open_fd

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
