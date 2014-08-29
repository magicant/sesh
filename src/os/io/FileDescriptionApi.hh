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

#ifndef INCLUDED_os_io_FileDescriptionApi_hh
#define INCLUDED_os_io_FileDescriptionApi_hh

#include "buildconfig.h"

#include <memory>
#include <system_error>
#include "common/variant.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"

namespace sesh {
namespace os {
namespace io {

class FileDescriptionApi {

public:

    /**
     * Returns the current status of the open file description associated with
     * the specified file descriptor.
     *
     * @return either a non-null pointer to a new file description status
     * instance or a non-zero system error code.
     */
    virtual
    common::variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
    getFileDescriptionStatus(const FileDescriptor &) const = 0;

    /**
     * Modifies the status of the open file description associated with the
     * specified file descriptor.
     *
     * An attribute in the specified status will be ignored if it is not
     * supported by the OS API. The access mode cannot be modified by this
     * function.
     *
     * The argument status instance must be obtained from the {@link
     * #getFileDescriptionStatus} function called for the same {@code *this}.
     */
    virtual std::error_code setFileDescriptionStatus(
            const FileDescriptor &, const FileDescriptionStatus &) const = 0;

}; // class FileDescriptionApi

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptionApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
