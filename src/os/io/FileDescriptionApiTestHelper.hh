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

#ifndef INCLUDED_os_io_FileDescriptionApiTestHelper_hh
#define INCLUDED_os_io_FileDescriptionApiTestHelper_hh

#include "buildconfig.h"

#include <system_error>
#include "os/io/FileDescriptionApi.hh"

namespace sesh {
namespace os {
namespace io {

class FileDescriptionApiDummy : public FileDescriptionApi {

    common::Variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
    getFileDescriptionStatus(const FileDescriptor &) const override {
        return std::error_code();
    }

    std::error_code setFileDescriptionStatus(
            const FileDescriptor &, const FileDescriptionStatus &) const
            override {
        return std::error_code();
    }

};

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptionApiTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
