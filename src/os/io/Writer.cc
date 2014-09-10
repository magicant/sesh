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
#include "Writer.hh"

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

using ResultPair = std::pair<non_blocking_file_descriptor, std::error_code>;

struct Writer {

    const WriterApi &api;
    class proactor &proactor;
    non_blocking_file_descriptor fd;
    std::vector<char> bytes;

    future<ResultPair> operator()(std::size_t bytesWritten) {
        auto i = bytes.begin();
        bytes.erase(i, i + bytesWritten);
        return write(api, proactor, std::move(fd), std::move(bytes));
    }

    future<ResultPair> operator()(std::error_code e) {
        return make_future<ResultPair>(std::move(fd), e);
    }

    future<ResultPair> operator()(trial<trigger> &&t) {
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

}; // struct Writer

} // namespace

future<ResultPair> write(
        const WriterApi &api,
        proactor &p,
        non_blocking_file_descriptor &&fd,
        std::vector<char> &&bytes) {
    if (bytes.empty())
        return make_future<ResultPair>(std::move(fd), std::error_code());

    auto trigger = writable_file_descriptor(fd.value());
    auto writer = Writer{api, p, std::move(fd), std::move(bytes)};
    return p.expect(trigger).then(std::move(writer)).unwrap();
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
