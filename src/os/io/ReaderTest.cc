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
#include <initializer_list>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "async/promise.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "common/variant.hh"
#include "os/event/Proactor.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Trigger.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptorTestHelper.hh"
#include "os/io/Reader.hh"
#include "os/io/ReaderApi.hh"

namespace {

using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::event::Proactor;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Trigger;
using sesh::os::io::FileDescriptor;
using sesh::os::io::NonBlockingFileDescriptor;
using sesh::os::io::ReaderApi;
using sesh::os::io::dummyNonBlockingFileDescriptor;
using sesh::os::io::read;

using ResultPair = std::pair<
        NonBlockingFileDescriptor,
        variant<std::vector<char>, std::error_code>>;

class UncallableReaderApi : public ReaderApi {

    ReadResult read(const FileDescriptor &, void *, std::size_t) const
            override {
        throw "unexpected read";
    }

}; // class UncallableReaderApi

class UncallableProactor : public Proactor {

    future<Trigger> expectImpl(std::vector<Trigger> &&) override {
        throw "unexpected expect";
    }

}; // class UncallableProactor

class EchoingProactor : public Proactor {

    future<Trigger> expectImpl(std::vector<Trigger> &&triggers) override {
        REQUIRE_FALSE(triggers.empty());
        return make_future<Trigger>(std::move(triggers.front()));
    }

}; // class EchoingProactor

namespace empty_read {

TEST_CASE("Read: empty") {
    const FileDescriptor::Value FD = 2;
    UncallableReaderApi api;
    UncallableProactor p;
    future<ResultPair> f = read(api, p, dummyNonBlockingFileDescriptor(FD), 0);

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        CHECK(r->second.value<std::vector<char>>().empty());
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace empty_read

namespace non_empty_read {

constexpr static FileDescriptor::Value FD = 3;

class ReadTestFixture : public ReaderApi, public Proactor {

private:

    mutable bool mIsReadyToRead = false;
    promise<Trigger> mPromise;

public:

    mutable std::vector<char> readableBytes;

    void setReadyToRead() {
        if (mIsReadyToRead)
            return;
        mIsReadyToRead = true;
        if (mPromise.is_valid())
            std::move(mPromise).set_result(ReadableFileDescriptor(FD));
    }

private:

    ReadResult read(const FileDescriptor &fd, void *buffer, std::size_t count)
            const override {
        CHECK(mIsReadyToRead);
        mIsReadyToRead = false;

        CHECK(fd.isValid());
        CHECK(fd.value() == FD);
        REQUIRE(buffer != nullptr);
        CHECK(count > 0);

        if (count > static_cast<std::size_t>(readableBytes.size()))
            count = static_cast<std::size_t>(readableBytes.size());

        auto begin = readableBytes.begin();
        auto end = begin + count;
        std::copy(begin, end, static_cast<char *>(buffer));
        readableBytes.erase(begin, end);

        return count;
    }

    future<Trigger> expectImpl(std::vector<Trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        Trigger &t = triggers.front();
        CHECK(t.tag() == t.tag<ReadableFileDescriptor>());
        CHECK(t.value<ReadableFileDescriptor>().value() == FD);

        auto pf = make_promise_future_pair<Trigger>();
        mPromise = std::move(pf.first);
        return std::move(pf.second);
    }

}; // class ReadTestFixture

TEST_CASE_METHOD(ReadTestFixture, "Read: reading less than available") {
    const char C = '&';
    readableBytes.assign(2, C);

    future<ResultPair> f = sesh::os::io::read(
            *this, *this, dummyNonBlockingFileDescriptor(FD), 1);

    bool called = false;
    std::move(f).then([C, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        REQUIRE(r->second.value<std::vector<char>>().size() == 1);
        CHECK(r->second.value<std::vector<char>>().at(0) == C);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(readableBytes.size() == 2);

    setReadyToRead();

    CHECK(called);
    CHECK(readableBytes.size() == 1);
}

TEST_CASE_METHOD(ReadTestFixture, "Read: reading less than buffer size") {
    std::string chars = "0123456789";
    std::vector<char> bytes(chars.begin(), chars.end());

    readableBytes = bytes;

    future<ResultPair> f = sesh::os::io::read(
            *this, *this, dummyNonBlockingFileDescriptor(FD), 11);

    bool called = false;
    std::move(f).then([&bytes, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        CHECK(r->second.value<std::vector<char>>() == bytes);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(readableBytes.size() == bytes.size());

    setReadyToRead();

    CHECK(called);
    CHECK(readableBytes.empty());
}

} // namespace non_empty_read

namespace domain_error {

class DomainErrorProactor : public Proactor {

    future<Trigger> expectImpl(std::vector<Trigger> &&) override {
        return make_failed_future_of<Trigger>(
                std::domain_error("expected error"));
    }

}; // class DomainErrorProactor

TEST_CASE("Read: domain error in proactor") {
    const FileDescriptor::Value FD = 2;
    UncallableReaderApi api;
    DomainErrorProactor p;
    future<ResultPair> f = read(
            api, p, dummyNonBlockingFileDescriptor(FD), {'C'});

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        REQUIRE(r->second.tag() == r->second.tag<std::error_code>());
        CHECK(r->second.value<std::error_code>() ==
                std::make_error_code(std::errc::too_many_files_open));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace domain_error

namespace read_error {

class ReadErrorApiStub : public ReaderApi {

    ReadResult read(const FileDescriptor &, void *, std::size_t) const
            override {
        return std::make_error_code(std::errc::io_error);
    }

}; // class ReadErrorApiStub

TEST_CASE("Read: read error") {
    const FileDescriptor::Value FD = 4;
    ReadErrorApiStub api;
    EchoingProactor proactor;
    future<ResultPair> f = sesh::os::io::read(
            api, proactor, dummyNonBlockingFileDescriptor(FD), {'A'});

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.isValid());
        CHECK(r->first.value() == FD);
        REQUIRE(r->second.tag() == r->second.tag<std::error_code>());
        CHECK(r->second.value<std::error_code>() ==
                std::make_error_code(std::errc::io_error));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace read_error

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
