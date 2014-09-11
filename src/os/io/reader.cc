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
#include "reader.hh"

#include <cassert>
#include <cstdint>
#include <utility>
#include "async/future.hh"
#include "common/trial.hh"
#include "common/variant.hh"
#include "os/event/proactor.hh"
#include "os/event/readable_file_descriptor.hh"
#include "os/event/trigger.hh"

using sesh::async::future;
using sesh::async::make_future;
using sesh::common::trial;
using sesh::common::variant;
using sesh::os::event::proactor;
using sesh::os::event::readable_file_descriptor;
using sesh::os::event::trigger;

namespace sesh {
namespace os {
namespace io {

namespace {

using result_pair = std::pair<
        non_blocking_file_descriptor,
        variant<std::vector<char>, std::error_code>>;

struct reader {

    const reader_api &api;
    non_blocking_file_descriptor fd;
    std::vector<char> buffer;

    result_pair operator()(std::size_t bytes_read) {
        buffer.resize(static_cast<std::vector<char>::size_type>(bytes_read));
        return result_pair(std::move(fd), std::move(buffer));
    }

    result_pair operator()(std::error_code e) {
        return result_pair(std::move(fd), std::move(e));
    }

    result_pair operator()(trial<trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &) {
            return operator()(
                    std::make_error_code(std::errc::too_many_files_open));
        }

        assert(t->value<readable_file_descriptor>().value() == fd.value());

        auto buffer_body = static_cast<void *>(buffer.data());
        auto size = static_cast<std::size_t>(buffer.size());
        return api.read(fd, buffer_body, size).apply(*this);
    }

}; // struct reader

} // namespace

future<result_pair> read(
        const reader_api &api,
        proactor &p,
        non_blocking_file_descriptor &&fd,
        std::vector<char>::size_type max_bytes_to_read) {
    if (max_bytes_to_read == 0)
        return make_future<result_pair>(std::move(fd), std::vector<char>());

    if (max_bytes_to_read > SIZE_MAX)
        max_bytes_to_read = SIZE_MAX;

    auto trigger = readable_file_descriptor(fd.value());
    return p.expect(trigger).then(
            reader{api, std::move(fd), std::vector<char>(max_bytes_to_read)});
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
