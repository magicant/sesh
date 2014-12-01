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

#ifndef INCLUDED_os_io_non_blocking_file_descriptor_hh
#define INCLUDED_os_io_non_blocking_file_descriptor_hh

#include "buildconfig.h"

#include <memory>
#include "os/io/file_description_api.hh"
#include "os/io/file_description_status.hh"
#include "os/io/file_descriptor.hh"

namespace sesh {
namespace os {
namespace io {

/**
 * A non-blocking file descriptor is a file descriptor with the non-blocking
 * mode enabled.
 *
 * A non-blocking file descriptor is constructed from a file descriptor, when
 * its non-blocking mode is set. When the non-blocking file descriptor is
 * {@link #release()}d, the non-blocking mode is reset.
 */
class non_blocking_file_descriptor {

private:

    const file_description_api &m_api;

    file_descriptor m_file_descriptor;
    std::unique_ptr<file_description_status> m_original_status;

public:

    /**
     * Returns a copy of the internal representation of this file descriptor.
     * The result is unspecified if this file descriptor is invalid.
     */
    file_descriptor::value_type value() const noexcept {
        return m_file_descriptor.value();
    }

    /** Checks if this file descriptor contains a valid value. */
    bool is_valid() const noexcept {
        return m_file_descriptor.is_valid();
    }

    operator const file_descriptor &() const noexcept {
        return m_file_descriptor;
    }

    /**
     * Constructs a new non-blocking file descriptor.
     *
     * The argument file descriptor is moved into the new non-blocking file
     * descriptor instance. If the file descriptor is valid, its non-blocking
     * flag is set using the API.
     *
     * @param api OS API to change the file descriptor's blocking mode with.
     * The API instance must be valid until the new non-blocking file
     * descriptor is destructed.
     * @param fd File descriptor whose non-blocking flag will be set.
     */
    non_blocking_file_descriptor(
            const file_description_api &api, file_descriptor &&fd);

    /** Move constructor. */
    non_blocking_file_descriptor(non_blocking_file_descriptor &&) noexcept =
            default;

    /**
     * Restores the non-blocking mode of the file descriptor to the original
     * and returns the file descriptor. This non-blocking file descriptor
     * becomes invalid after the function call.
     *
     * If the file description status was changed after the construction of
     * this non-blocking file descriptor, the change may be canceled by this
     * function.
     *
     * If the native file descriptor was replaced after the construction of
     * this non-blocking file descriptor, the new native file descriptor will
     * be affected by this function.
     */
    file_descriptor release();

    /**
     * Destructs the non-blocking file descriptor instance. The file descriptor
     * must be invalid on destruction; otherwise the behavior is undefined.
     */
    ~non_blocking_file_descriptor() = default;

}; // class non_blocking_file_descriptor

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_non_blocking_file_descriptor_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
