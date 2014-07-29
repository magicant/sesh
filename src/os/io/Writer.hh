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

#ifndef INCLUDED_os_io_Writer_hh
#define INCLUDED_os_io_Writer_hh

#include "buildconfig.h"

#include <system_error>
#include <utility>
#include "async/Future.hh"
#include "os/event/Proactor.hh"
#include "os/io/NonBlockingFileDescriptor.hh"
#include "os/io/WriterApi.hh"

namespace sesh {
namespace os {
namespace io {

/**
 * Writes the given byte array to the given file descriptor.
 *
 * When all the bytes are written successfully or the underlying API returns an
 * error, the returned future receives the result. The argument writer API and
 * proactor must be valid until then.
 *
 * The argument file descriptor must be kept valid (open) until the future
 * returned from this function receives the result. Otherwise, the behavior is
 * undefined. That means you must ensure validity of the file descriptors
 * before calling this function.
 *
 * The result is a pair of the target file descriptor and an error code. On
 * success, the error code is zero. A non-zero error code indicates an error,
 * but some bytes have been written before the error occurred.
 */
async::Future<std::pair<NonBlockingFileDescriptor, std::error_code>> write(
        const WriterApi &,
        event::Proactor &,
        NonBlockingFileDescriptor &&,
        std::vector<char> &&);

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_Writer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
