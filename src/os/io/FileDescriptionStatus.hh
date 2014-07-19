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

#ifndef INCLUDED_os_io_FileDescriptionStatus_hh
#define INCLUDED_os_io_FileDescriptionStatus_hh

#include "buildconfig.h"

#include <memory>
#include "os/io/FileDescriptionAccessMode.hh"
#include "os/io/FileDescriptionAttribute.hh"

namespace sesh {
namespace os {
namespace io {

/**
 * Represents the status of an open file description.
 *
 * A file description status is composed of exactly one access mode and a set
 * of any number of file description attributes. The access mode is not
 * modifiable.
 */
class FileDescriptionStatus {

public:

    virtual ~FileDescriptionStatus() = default;

    /** Returns the access mode in this status. */
    virtual FileDescriptionAccessMode accessMode() const noexcept = 0;

    /**
     * Checks if an open file description attribute is included in this status.
     */
    virtual bool test(FileDescriptionAttribute) const noexcept = 0;

    /** Adds/removes an open file description attribute to/from this status. */
    virtual FileDescriptionStatus &set(FileDescriptionAttribute, bool = true)
            noexcept = 0;

    /** Removes an open file description attribute from this status. */
    FileDescriptionStatus &reset(FileDescriptionAttribute a) noexcept {
        return set(a, false);
    }

    /** Clears all open file description attributes of this status. */
    virtual FileDescriptionStatus &resetAttributes() noexcept = 0;

    /** Creates a copy of this instance. */
    virtual std::unique_ptr<FileDescriptionStatus> clone() const = 0;

};

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptionStatus_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
