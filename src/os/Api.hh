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

#ifndef INCLUDED_os_Api_hh
#define INCLUDED_os_Api_hh

#include "buildconfig.h"

#include <memory>
#include <system_error>
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"

namespace sesh {
namespace os {

/** Abstraction of POSIX API. */
class Api {

    /**
     * Closes the given file descriptor. This function may block on some
     * conditions; refer to the POSIX standard for details.
     *
     * On success, the argument file descriptor is invalidated and a
     * default-constructed error condition is returned.
     *
     * On failure, the file descriptor may be left still valid.
     */
    virtual std::error_condition close(io::FileDescriptor &) const = 0;

    /** Returns a unique pointer to a new empty file descriptor set. */
    virtual std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet()
            const = 0;

}; // class Api

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_Api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
