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
#include "RealApi.hh"

#include <system_error>
#include "common/ErrnoHelper.hh"
#include "os/api.h"
#include "os/io/FileDescriptor.hh"

using sesh::common::errnoCondition;
using sesh::os::io::FileDescriptor;

namespace sesh {
namespace os {

std::error_condition RealApi::close(FileDescriptor &fd) const {
    if (sesh_osapi_close(fd.value()) == 0) {
        fd.clear();
        return std::error_condition();
    }

    std::error_condition ec = errnoCondition();
    if (ec == std::make_error_condition(std::errc::bad_file_descriptor))
        fd.clear();
    return ec;
}

} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
