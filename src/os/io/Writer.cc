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
#include "async/Future.hh"
#include "common/Try.hh"
#include "os/event/Proactor.hh"
#include "os/event/Trigger.hh"
#include "os/event/WritableFileDescriptor.hh"

using sesh::async::Future;
using sesh::async::createFuture;
using sesh::common::Try;
using sesh::os::event::Proactor;
using sesh::os::event::Trigger;
using sesh::os::event::WritableFileDescriptor;

namespace sesh {
namespace os {
namespace io {

namespace {

using ResultPair = std::pair<NonBlockingFileDescriptor, std::error_code>;

struct Writer {

    const WriterApi &api;
    Proactor &proactor;
    NonBlockingFileDescriptor fd;
    std::vector<char> bytes;

    Future<ResultPair> operator()(std::size_t bytesWritten) {
        auto i = bytes.begin();
        bytes.erase(i, i + bytesWritten);
        return write(api, proactor, std::move(fd), std::move(bytes));
    }

    Future<ResultPair> operator()(std::error_code e) {
        return createFuture<ResultPair>(std::move(fd), e);
    }

    Future<ResultPair> operator()(Try<Trigger> &&t) {
        try {
            *t;
        } catch (std::domain_error &e) {
            return operator()(
                    std::make_error_code(std::errc::too_many_files_open));
        }

        assert(t->value<WritableFileDescriptor>().value() == fd.value());

        auto r = api.write(
                fd,
                static_cast<const void *>(bytes.data()),
                bytes.size());
        return std::move(r).apply(*this);
    }

}; // struct Writer

} // namespace

Future<ResultPair> write(
        const WriterApi &api,
        Proactor &p,
        NonBlockingFileDescriptor &&fd,
        std::vector<char> &&bytes) {
    if (bytes.empty())
        return createFuture<ResultPair>(std::move(fd), std::error_code());

    auto trigger = WritableFileDescriptor(fd.value());
    auto writer = Writer{api, p, std::move(fd), std::move(bytes)};
    return p.expect(trigger).then(std::move(writer)).unwrap();
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
