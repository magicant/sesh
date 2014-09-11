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

#ifndef INCLUDED_os_io_file_description_status_hh
#define INCLUDED_os_io_file_description_status_hh

#include "buildconfig.h"

#include <memory>
#include "os/io/file_description_access_mode.hh"
#include "os/io/file_description_attribute.hh"

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
class file_description_status {

public:

    virtual ~file_description_status() = default;

    /** Returns the access mode in this status. */
    virtual file_description_access_mode access_mode() const noexcept = 0;

    /**
     * Checks if an open file description attribute is included in this status.
     */
    virtual bool test(file_description_attribute) const noexcept = 0;

    /** Adds/removes an open file description attribute to/from this status. */
    virtual file_description_status &set(
            file_description_attribute, bool = true) noexcept = 0;

    /** Removes an open file description attribute from this status. */
    file_description_status &reset(file_description_attribute a) noexcept {
        return set(a, false);
    }

    /** Clears all open file description attributes of this status. */
    virtual file_description_status &reset_attributes() noexcept = 0;

    /** Creates a copy of this instance. */
    virtual std::unique_ptr<file_description_status> clone() const = 0;

}; // class file_description_status

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_description_status_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
