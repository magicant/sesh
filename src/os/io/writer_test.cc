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

#include <algorithm>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include <utility>
#include "async/future.hh"
#include "async/promise.hh"
#include "catch.hpp"
#include "common/copy.hh"
#include "common/trial.hh"
#include "common/type_tag_test_helper.hh"
#include "os/event/proactor.hh"
#include "os/event/trigger.hh"
#include "os/event/writable_file_descriptor.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor_test_helper.hh"
#include "os/io/writer.hh"
#include "os/io/writer_api.hh"

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
using sesh::os::io::write;
using sesh::os::io::writer_api;

using result_pair = std::pair<non_blocking_file_descriptor, std::error_code>;

class uncallable_writer_api : public writer_api {

    write_result write(const file_descriptor &, const void *, std::size_t)
            const override {
        throw "unexpected write";
    }

}; // class uncallable_writer_api

class uncallable_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        throw "unexpected expect";
    }

}; // class uncallable_proactor

class echoing_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        return make_future<trigger>(std::move(triggers.front()));
    }

}; // class echoing_proactor

namespace empty_write {

TEST_CASE("Write: empty") {
    const file_descriptor::value_type fd = 2;
    uncallable_writer_api api;
    uncallable_proactor p;
    future<result_pair> f = write(
            api,
            p,
            dummy_non_blocking_file_descriptor(fd),
            std::vector<char>());

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace empty_write

namespace non_empty_write {

constexpr static file_descriptor::value_type fd_value = 3;

class write_test_fixture : public writer_api, public proactor {

private:

    mutable bool m_is_ready_to_write = false;
    promise<trigger> m_promise;

public:

    mutable std::vector<char> written_bytes;

    void set_ready_to_write() {
        if (m_is_ready_to_write)
            return;
        m_is_ready_to_write = true;
        if (m_promise.is_valid())
            std::move(m_promise).set_result(
                    writable_file_descriptor(fd_value));
    }

private:

    write_result write(
            const file_descriptor &fd, const void *bytes, std::size_t count)
            const override {
        CHECK(m_is_ready_to_write);
        m_is_ready_to_write = false;

        CHECK(fd.value() == fd_value);
        REQUIRE(bytes != nullptr);
        CHECK(count > 0);

        count = std::min(count, static_cast<std::size_t>(10));

        const char *bytes_begin = static_cast<const char *>(bytes);
        const char *bytes_end = bytes_begin + count;
        written_bytes.insert(written_bytes.end(), bytes_begin, bytes_end);

        return count;
    }

    future<trigger> expect_impl(std::vector<trigger> &&triggers) override {
        REQUIRE(triggers.size() == 1);
        trigger &t = triggers.front();
        CHECK(t.tag() == t.tag<writable_file_descriptor>());
        CHECK(t.value<writable_file_descriptor>().value() == fd_value);

        auto pf = make_promise_future_pair<trigger>();
        m_promise = std::move(pf.first);
        return std::move(pf.second);
    }

}; // class write_test_fixture

TEST_CASE_METHOD(write_test_fixture, "Write: one byte") {
    const char c = '!';
    future<result_pair> f = sesh::os::io::write(
            *this,
            *this,
            dummy_non_blocking_file_descriptor(fd_value),
            std::vector<char>{c});

    bool called = false;
    std::move(f).then([&called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd_value);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(written_bytes.empty());

    set_ready_to_write();

    CHECK(called);
    CHECK(written_bytes.size() == 1);
    CHECK(written_bytes.at(0) == c);
}

TEST_CASE_METHOD(write_test_fixture, "Write: 25 bytes in three writes") {
    std::string chars = "0123456789abcdefghijklmno";
    std::vector<char> bytes(chars.begin(), chars.end());
    future<result_pair> f = sesh::os::io::write(
            *this,
            *this,
            dummy_non_blocking_file_descriptor(fd_value),
            copy(bytes));

    bool called = false;
    std::move(f).then([&called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd_value);
        CHECK(r->second.value() == 0);
        r->first.release().clear();
        called = true;
    });
    CHECK_FALSE(called);
    CHECK(written_bytes.empty());

    set_ready_to_write();

    CHECK_FALSE(called);
    CHECK(written_bytes.size() == 10);

    set_ready_to_write();

    CHECK_FALSE(called);
    CHECK(written_bytes.size() == 20);

    set_ready_to_write();

    CHECK(called);
    CHECK(written_bytes == bytes);
}

} // namespace non_empty_write

namespace domain_error {

class domain_error_proactor : public proactor {

    future<trigger> expect_impl(std::vector<trigger> &&) override {
        return make_failed_future_of<trigger>(
                std::domain_error("expected error"));
    }

}; // class domain_error_proactor

TEST_CASE("Write: domain error in proactor") {
    const file_descriptor::value_type fd = 2;
    uncallable_writer_api api;
    domain_error_proactor p;
    future<result_pair> f = write(
            api, p, dummy_non_blocking_file_descriptor(fd), {'C'});

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
        CHECK(r->second ==
                std::make_error_code(std::errc::too_many_files_open));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace domain_error

namespace write_error {

class write_error_api : public writer_api {

    write_result write(const file_descriptor &, const void *, std::size_t)
            const override {
        return std::make_error_code(std::errc::io_error);
    }

}; // class write_error_api

TEST_CASE("Write: write error") {
    const file_descriptor::value_type fd = 4;
    write_error_api api;
    echoing_proactor proactor;
    future<result_pair> f = sesh::os::io::write(
            api, proactor, dummy_non_blocking_file_descriptor(fd), {'A'});

    bool called = false;
    std::move(f).then([fd, &called](trial<result_pair> &&r) {
        REQUIRE(r.has_value());
        CHECK(r->first.is_valid());
        CHECK(r->first.value() == fd);
        CHECK(r->second == std::make_error_code(std::errc::io_error));
        r->first.release().clear();
        called = true;
    });
    CHECK(called);
}

} // namespace write_error

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
