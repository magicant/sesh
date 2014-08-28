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

#ifndef INCLUDED_os_io_FileDescriptorOpenMode_hh
#define INCLUDED_os_io_FileDescriptorOpenMode_hh

#include "buildconfig.h"

#include "common/enum_traits.hh"

namespace sesh {

namespace os {
namespace io {

/** This enum class defines the behavior of the file open API. */
enum class FileDescriptorOpenMode {
    CLOSE_ON_EXEC,
    CREATE,
    DIRECTORY,
    EXCLUSIVE,
    NO_CONTROLLING_TERMINAL,
    NO_FOLLOW,
    TRUNCATE,
    TTY_INITIALIZE,
};

} // namespace io
} // namespace os

namespace common {

template<>
class enum_traits<os::io::FileDescriptorOpenMode> {
public:
    constexpr static os::io::FileDescriptorOpenMode max =
            os::io::FileDescriptorOpenMode::TTY_INITIALIZE;
};

} // namespace common

} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptorOpenMode_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
