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

#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include "async/Future.hh"
#include "async/Promise.hh"
#include "common/Copy.hh"
#include "common/Try.hh"
#include "os/event/Proactor.hh"
#include "os/event/Trigger.hh"
#include "os/event/WritableFileDescriptor.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptorTestHelper.hh"
#include "os/io/Writer.hh"
#include "os/io/WriterApi.hh"

namespace {

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createPromiseFuturePair;
using sesh::common::Try;
using sesh::common::copy;
using sesh::os::event::Proactor;
using sesh::os::event::Trigger;
using sesh::os::event::WritableFileDescriptor;
using sesh::os::io::FileDescriptor;
using sesh::os::io::NonBlockingFileDescriptor;
using sesh::os::io::WriterApi;
using sesh::os::io::dummyNonBlockingFileDescriptor;
using sesh::os::io::write;

using ResultPair = std::pair<NonBlockingFileDescriptor, std::error_code>;

namespace empty_write {

class UncallableWriterApi : public WriterApi {

    WriteResult write(
            const FileDescriptor &, const void *, std::size_t) const override {
        throw "unexpected write";
    }

}; // class UncallableWriterApi

class UncallableProactor : public Proactor {

    Future<Trigger> expectImpl(std::vector<Trigger> &&) override {
        throw "unexpected expect";
    }

}; // class UncallableProactor

TEST_CASE("Write: empty") {
    const FileDescriptor::Value FD = 2;
    UncallableWriterApi api;
    UncallableProactor p;
    Future<ResultPair> f = write(
            api, p, dummyNonBlockingFileDescriptor(FD), std::vector<char>());

    bool called = false;
    std::move(f).then([FD, &called](Try<ResultPair> &&r) {
        REQUIRE(r.hasValue());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace empty_write

namespace non_empty_write {

constexpr static FileDescriptor::Value FD = 3;

class WriteTestFixture : public WriterApi, public Proactor {

private:

    mutable bool mIsReadyToWrite = false;
    Promise<Trigger> mPromise;

public:

    mutable std::vector<char> writtenBytes;

    void setReadyToWrite() {
        if (mIsReadyToWrite)
            return;
        mIsReadyToWrite = true;
        if (mPromise.isValid())
            std::move(mPromise).setResult(WritableFileDescriptor(FD));
    }

private:

    WriteResult write(
            const FileDescriptor &fd, const void *bytes, std::size_t count)
            const override {
        CHECK(mIsReadyToWrite);
        mIsReadyToWrite = false;

        CHECK(fd.value() == FD);
        REQUIRE(bytes != nullptr);
        CHECK(count > 0);

        count = std::min(count, static_cast<std::size_t>(10));

        const char *bytesBegin = static_cast<const char *>(bytes);
        const char *bytesEnd = bytesBegin + count;
        writtenBytes.insert(writtenBytes.end(), bytesBegin, bytesEnd);

        return count;
    }

    Future<Trigger> expectImpl(std::vector<Trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        Trigger &t = triggers.front();
        CHECK(t.index() == t.index<WritableFileDescriptor>());
        CHECK(t.value<WritableFileDescriptor>().value() == FD);

        auto pf = createPromiseFuturePair<Trigger>();
        mPromise = std::move(pf.first);
        return std::move(pf.second);
    }

}; // class WriteTestFixture

TEST_CASE_METHOD(WriteTestFixture, "Write: one byte") {
    const char C = '!';
    Future<ResultPair> f = sesh::os::io::write(
            *this,
            *this,
            dummyNonBlockingFileDescriptor(FD),
            std::vector<char>{C});

    bool called = false;
    std::move(f).then([&called](Try<ResultPair> &&r) {
        REQUIRE(r.hasValue());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(writtenBytes.empty());

    setReadyToWrite();

    CHECK(called);
    CHECK(writtenBytes.size() == 1);
    CHECK(writtenBytes.at(0) == C);
}

TEST_CASE_METHOD(WriteTestFixture, "Write: 25 bytes in three writes") {
    std::string chars = "0123456789abcdefghijklmno";
    std::vector<char> bytes(chars.begin(), chars.end());
    Future<ResultPair> f = sesh::os::io::write(
            *this, *this, dummyNonBlockingFileDescriptor(FD), copy(bytes));

    bool called = false;
    std::move(f).then([&called](Try<ResultPair> &&r) {
        REQUIRE(r.hasValue());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(writtenBytes.empty());

    setReadyToWrite();

    CHECK_FALSE(called);
    CHECK(writtenBytes.size() == 10);

    setReadyToWrite();

    CHECK_FALSE(called);
    CHECK(writtenBytes.size() == 20);

    setReadyToWrite();

    CHECK(called);
    CHECK(writtenBytes == bytes);
}

} // namespace non_empty_write

namespace error {

// FIXME more tests

} // namespace error

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
