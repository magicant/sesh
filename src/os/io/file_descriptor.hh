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

#ifndef INCLUDED_os_io_file_descriptor_hh
#define INCLUDED_os_io_file_descriptor_hh

#include "buildconfig.h"

#include <cassert>
#include <exception>
#include <utility>

namespace sesh {
namespace os {
namespace io {

/**
 * A file descriptor is an abstract reference to an open file description.
 *
 * An instance of this class may or may not represent a POSIX file descriptor.
 * The instance is <em>valid</em> when it contains an internal representation
 * that is <em>expected</em> to be a valid POSIX file descriptor. (The value
 * might not be an actual valid POSIX file descriptor.)
 */
class file_descriptor {

public:

    /** The internal representation type. */
    using value_type = int;

    /** An internal representation of an invalid file descriptor. */
    constexpr static value_type invalid_value = -1;

private:

    value_type m_value;

public:

    /**
     * Returns a copy of the internal representation of this file descriptor.
     * The result is unspecified if this file descriptor is invalid.
     */
    value_type value() const noexcept {
        return m_value;
    }

    /** Checks if this file descriptor contains a valid value. */
    bool is_valid() const noexcept {
        return m_value >= 0;
    }

    /**
     * Clears the internal representation and makes this file descriptor
     * instance invalid without affecting the actual POSIX file descriptor.
     */
    void clear() noexcept {
        m_value = invalid_value;
    }

    /** Swaps this file descriptor with the argument. */
    void swap(file_descriptor &that) noexcept {
        std::swap(this->m_value, that.m_value);
    }

    /** Constructs an invalid file descriptor. */
    constexpr file_descriptor() noexcept : m_value(invalid_value) { }

    /**
     * Constructs a file descriptor of the given internal representation. A
     * negative internal representation value will be considered invalid.
     */
    constexpr explicit file_descriptor(value_type value) noexcept :
            m_value(value) { }

    /** Move constructor, which invalidates the other file descriptor. */
    file_descriptor(file_descriptor &&fd) noexcept : m_value(fd.value()) {
        fd.clear();
    }

    /** Move assignment, which swaps this and the other file descriptors. */
    file_descriptor &operator=(file_descriptor &&fd) noexcept {
        swap(fd);
        return *this;
    }

#ifndef NDEBUG
    /**
     * Destructs this file descriptor. The file descriptor must be invalid on
     * destruction. Destruction of a valid file descriptor is considered a
     * serious resource leak and results in termination of the whole program.
     */
    ~file_descriptor() noexcept {
        assert(!is_valid());
    }
#endif

}; // class file_descriptor

/** Swaps two file descriptors. */
inline void swap(file_descriptor &l, file_descriptor &r) noexcept {
    l.swap(r);
}

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_descriptor_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
