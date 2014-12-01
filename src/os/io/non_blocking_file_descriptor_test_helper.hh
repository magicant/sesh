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

#ifndef INCLUDED_os_io_non_blocking_file_descriptor_test_helper_hh
#define INCLUDED_os_io_non_blocking_file_descriptor_test_helper_hh

#include "buildconfig.h"

#include <system_error>
#include <utility>
#include "os/io/file_description_api_test_helper.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/non_blocking_file_descriptor.hh"

namespace sesh {
namespace os {
namespace io {

inline auto dummy_non_blocking_file_descriptor(file_descriptor &&fd)
        -> non_blocking_file_descriptor {
    static file_description_api_dummy api;
    return non_blocking_file_descriptor(api, std::move(fd));
}

inline auto dummy_non_blocking_file_descriptor(file_descriptor::value_type fd)
        -> non_blocking_file_descriptor {
    return dummy_non_blocking_file_descriptor(file_descriptor(fd));
}

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_non_blocking_file_descriptor_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
