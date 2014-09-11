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
#include "os/event/proactor.hh"
#include "os/event/readable_file_descriptor.hh"
#include "os/event/trigger.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor_test_helper.hh"
#include "os/io/reader.hh"
#include "os/io/ReaderApi.hh"

namespace {

using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::event::proactor;
using sesh::os::event::readable_file_descriptor;
using sesh::os::event::trigger;
using sesh::os::io::dummy_non_blocking_file_descriptor;
using sesh::os::io::file_descriptor;
using sesh::os::io::non_blocking_file_descriptor;
using sesh::os::io::ReaderApi;
using sesh::os::io::read;

using result_pair = std::pair<
        non_blocking_file_descriptor,
        variant<std::vector<char>, std::error_code>>;

class uncallable_reader_api : public ReaderApi {

    ReadResult read(const file_descriptor &, void *, std::size_t) const
            override {
        throw "unexpected read";
    }

}; // class uncallable_reader_api

class uncallable_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        throw "unexpected expect";
    }

}; // class uncallable_proactor

class echoing_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE_FALSE(triggers.empty());
        return make_future<trigger>(std::move(triggers.front()));
    }

}; // class echoing_proactor

namespace empty_read {

TEST_CASE("Read: empty") {
    const file_descriptor::value_type fd = 2;
    uncallable_reader_api api;
    uncallable_proactor p;
    future<result_pair> f =
            read(api, p, dummy_non_blocking_file_descriptor(fd), 0);

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        CHECK(r->second.value<std::vector<char>>().empty());
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace empty_read

namespace non_empty_read {

constexpr static file_descriptor::value_type fd_value = 3;

class read_test_fixture : public ReaderApi, public proactor {

private:

    mutable bool m_is_ready_to_read = false;
    promise<trigger> m_promise;

public:

    mutable std::vector<char> readable_bytes;

    void set_ready_to_read() {
        if (m_is_ready_to_read)
            return;
        m_is_ready_to_read = true;
        if (m_promise.is_valid())
            std::move(m_promise).set_result(
                    readable_file_descriptor(fd_value));
    }

private:

    ReadResult read(const file_descriptor &fd, void *buffer, std::size_t count)
            const override {
        CHECK(m_is_ready_to_read);
        m_is_ready_to_read = false;

        CHECK(fd.is_valid());
        CHECK(fd.value() == fd_value);
        REQUIRE(buffer != nullptr);
        CHECK(count > 0);

        if (count > static_cast<std::size_t>(readable_bytes.size()))
            count = static_cast<std::size_t>(readable_bytes.size());

        auto begin = readable_bytes.begin();
        auto end = begin + count;
        std::copy(begin, end, static_cast<char *>(buffer));
        readable_bytes.erase(begin, end);

        return count;
    }

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        trigger &t = triggers.front();
        CHECK(t.tag() == t.tag<readable_file_descriptor>());
        CHECK(t.value<readable_file_descriptor>().value() == fd_value);

        auto pf = make_promise_future_pair<trigger>();
        m_promise = std::move(pf.first);
        return std::move(pf.second);
    }

}; // class read_test_fixture

TEST_CASE_METHOD(read_test_fixture, "Read: reading less than available") {
    const char C = '&';
    readable_bytes.assign(2, C);

    future<result_pair> f = sesh::os::io::read(
            *this, *this, dummy_non_blocking_file_descriptor(fd_value), 1);

    bool called = false;
    std::move(f).then([C, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd_value);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        REQUIRE(r->second.value<std::vector<char>>().size() == 1);
        CHECK(r->second.value<std::vector<char>>().at(0) == C);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(readable_bytes.size() == 2);

    set_ready_to_read();

    CHECK(called);
    CHECK(readable_bytes.size() == 1);
}

TEST_CASE_METHOD(read_test_fixture, "Read: reading less than buffer size") {
    std::string chars = "0123456789";
    std::vector<char> bytes(chars.begin(), chars.end());

    readable_bytes = bytes;

    future<result_pair> f = sesh::os::io::read(
            *this, *this, dummy_non_blocking_file_descriptor(fd_value), 11);

    bool called = false;
    std::move(f).then([&bytes, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd_value);
        REQUIRE(r->second.tag() == r->second.tag<std::vector<char>>());
        CHECK(r->second.value<std::vector<char>>() == bytes);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(readable_bytes.size() == bytes.size());

    set_ready_to_read();

    CHECK(called);
    CHECK(readable_bytes.empty());
}

} // namespace non_empty_read

namespace domain_error {

class domain_error_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        return make_failed_future_of<trigger>(
                std::domain_error("expected error"));
    }

}; // class domain_error_proactor

TEST_CASE("Read: domain error in proactor") {
    const file_descriptor::value_type fd = 2;
    uncallable_reader_api api;
    domain_error_proactor p;
    future<result_pair> f = read(
            api, p, dummy_non_blocking_file_descriptor(fd), {'C'});

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
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

class read_error_api_stub : public ReaderApi {

    ReadResult read(const file_descriptor &, void *, std::size_t) const
            override {
        return std::make_error_code(std::errc::io_error);
    }

}; // class read_error_api_stub

TEST_CASE("Read: read error") {
    const file_descriptor::value_type fd = 4;
    read_error_api_stub api;
    echoing_proactor proactor;
    future<result_pair> f = sesh::os::io::read(
            api, proactor, dummy_non_blocking_file_descriptor(fd), {'A'});

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
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
