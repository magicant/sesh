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

#ifndef INCLUDED_os_io_file_descriptor_open_mode_hh
#define INCLUDED_os_io_file_descriptor_open_mode_hh

#include "buildconfig.h"

#include "common/enum_traits.hh"

namespace sesh {

namespace os {
namespace io {

/** This enum class defines the behavior of the file open API. */
enum class file_descriptor_open_mode {
    close_on_exec,
    create,
    directory,
    exclusive,
    no_controlling_terminal,
    no_follow,
    truncate,
    tty_initialize,
};

} // namespace io
} // namespace os

namespace common {

template<>
class enum_traits<os::io::file_descriptor_open_mode> {
public:
    constexpr static os::io::file_descriptor_open_mode max =
            os::io::file_descriptor_open_mode::tty_initialize;
};

} // namespace common

} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_descriptor_open_mode_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
