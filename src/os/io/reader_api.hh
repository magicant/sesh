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

#ifndef INCLUDED_os_io_reader_api_hh
#define INCLUDED_os_io_reader_api_hh

#include "buildconfig.h"

#include <cstddef>
#include <system_error>
#include "common/variant.hh"
#include "os/io/file_descriptor.hh"

namespace sesh {
namespace os {
namespace io {

/** Abstraction of POSIX API for reading. */
class reader_api {

public:

    using read_result = common::variant<std::size_t, std::error_code>;

    /**
     * Reads bytes from the given file descriptor. This function may block on
     * some conditions; refer to the POSIX standard for details.
     *
     * On success, the number of actually read bytes is returned. On failure, a
     * non-zero error code is returned.
     */
    virtual read_result read(const file_descriptor &, void *, std::size_t)
            const = 0;

}; // class reader_api

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_reader_api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
