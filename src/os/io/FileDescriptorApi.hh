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

#ifndef INCLUDED_os_io_FileDescriptorApi_hh
#define INCLUDED_os_io_FileDescriptorApi_hh

#include "buildconfig.h"

#include <memory>
#include <system_error>
#include "common/EnumSet.hh"
#include "common/Variant.hh"
#include "os/io/FileDescriptionAccessMode.hh"
#include "os/io/FileDescriptionAttribute.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorOpenMode.hh"
#include "os/io/FileMode.hh"

namespace sesh {
namespace os {
namespace io {

class FileDescriptorApi {

public:

    /**
     * Returns the current status of the open file description associated with
     * the specified file descriptor.
     *
     * @return either a non-null pointer to a new file description status
     * instance or a non-zero system error code.
     */
    virtual
    common::Variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
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

    /**
     * Opens a file. A new open file description and file descriptor are
     * created if successful. A non-zero error code is returned on failure.
     *
     * The file mode set argument is ignored unless the CREATE flag is included
     * in the file descriptor open mode set argument.
     */
    virtual common::Variant<FileDescriptor, std::error_code> open(
            const char *path,
            FileDescriptionAccessMode,
            common::EnumSet<FileDescriptionAttribute>,
            common::EnumSet<FileDescriptorOpenMode>,
            common::EnumSet<FileMode>) const = 0;

    /**
     * Closes the given file descriptor. This function may block on some
     * conditions; refer to the POSIX standard for details.
     *
     * On success, the argument file descriptor is invalidated and a
     * default-constructed error condition is returned.
     *
     * On failure, the file descriptor may be left still valid.
     */
    virtual std::error_code close(FileDescriptor &) const = 0;

}; // class FileDescriptorApi

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptorApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
