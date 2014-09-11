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

#ifndef INCLUDED_os_io_file_descriptor_set_hh
#define INCLUDED_os_io_file_descriptor_set_hh

#include "buildconfig.h"

#include "os/io/file_descriptor.hh"

namespace sesh {
namespace os {
namespace io {

/** A file descriptor set contains zero or more file descriptors. */
class file_descriptor_set {

public:

    file_descriptor_set() = default;
    file_descriptor_set(const file_descriptor_set &) = default;
    file_descriptor_set(file_descriptor_set &&) = default;
    file_descriptor_set &operator=(const file_descriptor_set &) = default;
    file_descriptor_set &operator=(file_descriptor_set &&) = default;
    virtual ~file_descriptor_set() = default;

    /** Returns the maximum value that can be contained in this set. */
    virtual file_descriptor::value_type max_value() const = 0;

    /** Checks if the given file descriptor is included in this set. */
    virtual bool test(file_descriptor::value_type) const = 0;

    /**
     * Adds/removes a file descriptor to/from this set.
     * @throws std::domain_error the value is too large.
     */
    virtual file_descriptor_set &set(file_descriptor::value_type, bool = true)
            = 0;

    /** Removes a file descriptor from this set. */
    file_descriptor_set &reset(file_descriptor::value_type fd) {
        return set(fd, false);
    }

    /** Clears this set. */
    virtual file_descriptor_set &reset() = 0;

    /** Reference to a single entry of a set. */
    class reference {

    private:

        file_descriptor_set &m_set;
        file_descriptor::value_type m_value;

    public:

        reference(file_descriptor_set &set, file_descriptor::value_type value)
                noexcept : m_set(set), m_value(value) { }

        reference &operator=(bool value) {
            m_set.set(m_value, value);
            return *this;
        }

        operator bool() {
            return m_set.test(m_value);
        }

        bool operator~() {
            return !*this;
        }

    }; // class reference

    /** Returns a reference to the specified entry of this set. */
    reference operator[](file_descriptor::value_type fd) noexcept {
        return reference(*this, fd);
    }

    /** Alias to {@link #test}. */
    bool operator[](file_descriptor::value_type fd) const {
        return test(fd);
    }

}; // class file_descriptor_set

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_descriptor_set_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
