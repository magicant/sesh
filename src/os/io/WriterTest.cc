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
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include <utility>
#include "async/future.hh"
#include "async/promise.hh"
#include "common/copy.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/proactor.hh"
#include "os/event/trigger.hh"
#include "os/event/writable_file_descriptor.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor_test_helper.hh"
#include "os/io/Writer.hh"
#include "os/io/WriterApi.hh"

namespace {

using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::copy;
using sesh::common::trial;
using sesh::os::event::proactor;
using sesh::os::event::trigger;
using sesh::os::event::writable_file_descriptor;
using sesh::os::io::dummy_non_blocking_file_descriptor;
using sesh::os::io::file_descriptor;
using sesh::os::io::non_blocking_file_descriptor;
using sesh::os::io::WriterApi;
using sesh::os::io::write;

using ResultPair = std::pair<non_blocking_file_descriptor, std::error_code>;

class UncallableWriterApi : public WriterApi {

    WriteResult write(const file_descriptor &, const void *, std::size_t) const
            override {
        throw "unexpected write";
    }

}; // class UncallableWriterApi

class UncallableProactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        throw "unexpected expect";
    }

}; // class UncallableProactor

class EchoingProactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        return make_future<trigger>(std::move(triggers.front()));
    }

}; // class EchoingProactor

namespace empty_write {

TEST_CASE("Write: empty") {
    const file_descriptor::value_type FD = 2;
    UncallableWriterApi api;
    UncallableProactor p;
    future<ResultPair> f = write(
            api,
            p,
            dummy_non_blocking_file_descriptor(FD),
            std::vector<char>());

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == FD);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace empty_write

namespace non_empty_write {

constexpr static file_descriptor::value_type FD = 3;

class WriteTestFixture : public WriterApi, public proactor {

private:

    mutable bool mIsReadyToWrite = false;
    promise<trigger> mPromise;

public:

    mutable std::vector<char> writtenBytes;

    void setReadyToWrite() {
        if (mIsReadyToWrite)
            return;
        mIsReadyToWrite = true;
        if (mPromise.is_valid())
            std::move(mPromise).set_result(writable_file_descriptor(FD));
    }

private:

    WriteResult write(
            const file_descriptor &fd, const void *bytes, std::size_t count)
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

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        trigger &t = triggers.front();
        CHECK(t.tag() == t.tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == FD);

        auto pf = make_promise_future_pair<trigger>();
        mPromise = std::move(pf.first);
        return std::move(pf.second);
    }

}; // class WriteTestFixture

TEST_CASE_METHOD(WriteTestFixture, "Write: one byte") {
    const char C = '!';
    future<ResultPair> f = sesh::os::io::write(
            *this,
            *this,
            dummy_non_blocking_file_descriptor(FD),
            std::vector<char>{C});

    bool called = false;
    std::move(f).then([&called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
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
    future<ResultPair> f = sesh::os::io::write(
            *this, *this, dummy_non_blocking_file_descriptor(FD), copy(bytes));

    bool called = false;
    std::move(f).then([&called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
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

namespace domain_error {

class DomainErrorProactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        return make_failed_future_of<trigger>(
                std::domain_error("expected error"));
    }

}; // class DomainErrorProactor

TEST_CASE("Write: domain error in proactor") {
    const file_descriptor::value_type FD = 2;
    UncallableWriterApi api;
    DomainErrorProactor p;
    future<ResultPair> f = write(
            api, p, dummy_non_blocking_file_descriptor(FD), {'C'});

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == FD);
        CHECK(r->second ==
                std::make_error_code(std::errc::too_many_files_open));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace domain_error

namespace write_error {

class WriteErrorApi : public WriterApi {

    WriteResult write(const file_descriptor &, const void *, std::size_t) const
            override {
        return std::make_error_code(std::errc::io_error);
    }

}; // class WriteErrorApi

TEST_CASE("Write: write error") {
    const file_descriptor::value_type FD = 4;
    WriteErrorApi api;
    EchoingProactor proactor;
    future<ResultPair> f = sesh::os::io::write(
            api, proactor, dummy_non_blocking_file_descriptor(FD), {'A'});

    bool called = false;
    std::move(f).then([FD, &called](trial<ResultPair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == FD);
        CHECK(r->second == std::make_error_code(std::errc::io_error));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace write_error

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
