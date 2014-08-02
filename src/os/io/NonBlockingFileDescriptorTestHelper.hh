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

#ifndef INCLUDED_os_io_NonBlockingFileDescriptorTestHelper_hh
#define INCLUDED_os_io_NonBlockingFileDescriptorTestHelper_hh

#include "buildconfig.h"

#include <system_error>
#include <utility>
#include "os/io/FileDescriptionApiTestHelper.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/NonBlockingFileDescriptor.hh"

namespace sesh {
namespace os {
namespace io {

inline auto dummyNonBlockingFileDescriptor(FileDescriptor &&fd)
        -> NonBlockingFileDescriptor {
    static FileDescriptionApiDummy api;
    return NonBlockingFileDescriptor(api, std::move(fd));
}

inline auto dummyNonBlockingFileDescriptor(FileDescriptor::Value fd)
        -> NonBlockingFileDescriptor {
    return dummyNonBlockingFileDescriptor(FileDescriptor(fd));
}

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_NonBlockingFileDescriptorTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
