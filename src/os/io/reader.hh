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

#ifndef INCLUDED_os_io_reader_hh
#define INCLUDED_os_io_reader_hh

#include "buildconfig.h"

#include <system_error>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/variant.hh"
#include "os/event/proactor.hh"
#include "os/io/non_blocking_file_descriptor.hh"
#include "os/io/ReaderApi.hh"

namespace sesh {
namespace os {
namespace io {

/**
 * Reads some bytes from the given file descriptor.
 *
 * When some bytes are read successfully or the underlying API fails with an
 * error, the returned future receives the result. The argument reader API and
 * proactor must be valid until then.
 *
 * The argument file descriptor must be kept valid (open) until the future
 * returned from this function receives the result. Otherwise, the behavior is
 * undefined. That means you must ensure validity of the file descriptors
 * before calling this function.
 *
 * The result is (a copy of) the argument file descriptor and either a vector
 * of bytes that were successfully read or a non-zero error code. The vector is
 * empty if and only if {@code max_bytes_to_read} is zero or the file
 * descriptor is at the end of file.
 *
 * @param max_bytes_to_read The maximum number of bytes to read. Note that the
 * actual number of bytes returned may be smaller.
 */
auto read(
        const ReaderApi &,
        event::proactor &,
        non_blocking_file_descriptor &&,
        std::vector<char>::size_type max_bytes_to_read)
        -> async::future<std::pair<
                non_blocking_file_descriptor,
                common::variant<std::vector<char>, std::error_code>>>;

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_reader_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
