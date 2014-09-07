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
#include "Reader.hh"

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

using ResultPair = std::pair<
        NonBlockingFileDescriptor,
        variant<std::vector<char>, std::error_code>>;

struct Reader {

    const ReaderApi &api;
    NonBlockingFileDescriptor fd;
    std::vector<char> buffer;

    ResultPair operator()(std::size_t bytesRead) {
        buffer.resize(static_cast<std::vector<char>::size_type>(bytesRead));
        return ResultPair(std::move(fd), std::move(buffer));
    }

    ResultPair operator()(std::error_code e) {
        return ResultPair(std::move(fd), std::move(e));
    }

    ResultPair operator()(trial<trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &) {
            return operator()(
                    std::make_error_code(std::errc::too_many_files_open));
        }

        assert(t->value<readable_file_descriptor>().value() == fd.value());

        auto bufferBody = static_cast<void *>(buffer.data());
        auto size = static_cast<std::size_t>(buffer.size());
        return api.read(fd, bufferBody, size).apply(*this);
    }

}; // struct Reader

} // namespace

future<ResultPair> read(
        const ReaderApi &api,
        proactor &p,
        NonBlockingFileDescriptor &&fd,
        std::vector<char>::size_type maxBytesToRead) {
    if (maxBytesToRead == 0)
        return make_future<ResultPair>(std::move(fd), std::vector<char>());

    if (maxBytesToRead > SIZE_MAX)
        maxBytesToRead = SIZE_MAX;

    auto trigger = readable_file_descriptor(fd.value());
    return p.expect(trigger).then(
            Reader{api, std::move(fd), std::vector<char>(maxBytesToRead)});
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
