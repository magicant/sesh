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
#include "writer.hh"

#include <cassert>
#include <stdexcept>
#include <system_error>
#include <utility>
#include "async/future.hh"
#include "common/trial.hh"
#include "os/event/proactor.hh"
#include "os/event/trigger.hh"
#include "os/event/writable_file_descriptor.hh"

using sesh::async::future;
using sesh::async::make_future;
using sesh::common::trial;
using sesh::os::event::proactor;
using sesh::os::event::trigger;
using sesh::os::event::writable_file_descriptor;

namespace sesh {
namespace os {
namespace io {

namespace {

using result_pair = std::pair<non_blocking_file_descriptor, std::error_code>;

struct writer {

    const writer_api &api;
    class proactor &proactor;
    non_blocking_file_descriptor fd;
    std::vector<char> bytes;

    future<result_pair> operator()(std::size_t bytes_written) {
        auto i = bytes.begin();
        bytes.erase(i, i + bytes_written);
        return write(api, proactor, std::move(fd), std::move(bytes));
    }

    future<result_pair> operator()(std::error_code e) {
        return make_future<result_pair>(std::move(fd), e);
    }

    future<result_pair> operator()(trial<trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &e) {
            return operator()(
                    std::make_error_code(std::errc::too_many_files_open));
        }

        assert(t->value<writable_file_descriptor>().value() == fd.value());

        auto r = api.write(
                fd,
                static_cast<const void *>(bytes.data()),
                bytes.size());
        return std::move(r).apply(*this);
    }

}; // struct writer

} // namespace

future<result_pair> write(
        const writer_api &api,
        proactor &p,
        non_blocking_file_descriptor &&fd,
        std::vector<char> &&bytes) {
    if (bytes.empty())
        return make_future<result_pair>(std::move(fd), std::error_code());

    auto trigger = writable_file_descriptor(fd.value());
    auto w = writer{api, p, std::move(fd), std::move(bytes)};
    return p.expect(trigger).then(std::move(w)).unwrap();
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
