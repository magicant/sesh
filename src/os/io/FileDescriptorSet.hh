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

#ifndef INCLUDED_os_io_FileDescriptorSet_hh
#define INCLUDED_os_io_FileDescriptorSet_hh

#include "buildconfig.h"

#include "os/io/FileDescriptor.hh"

namespace sesh {
namespace os {
namespace io {

/** A file descriptor set contains zero or more file descriptors. */
class FileDescriptorSet {

public:

    FileDescriptorSet() = default;
    FileDescriptorSet(const FileDescriptorSet &) = default;
    FileDescriptorSet(FileDescriptorSet &&) = default;
    FileDescriptorSet &operator=(const FileDescriptorSet &) = default;
    FileDescriptorSet &operator=(FileDescriptorSet &&) = default;
    virtual ~FileDescriptorSet() = default;

    /** Checks if the given file descriptor is included in this set. */
    virtual bool test(FileDescriptor::Value) const = 0;

    /** Adds/removes a file descriptor to/from this set. */
    virtual FileDescriptorSet &set(FileDescriptor::Value, bool = true) = 0;

    /** Removes a file descriptor from this set. */
    FileDescriptorSet &reset(FileDescriptor::Value fd) {
        return set(fd, false);
    }

    /** Clears this set. */
    virtual FileDescriptorSet &reset() = 0;

    /** Reference to a single entry of a set. */
    class Reference {

    private:

        FileDescriptorSet &mSet;
        FileDescriptor::Value mValue;

    public:

        Reference(FileDescriptorSet &set, FileDescriptor::Value value)
                noexcept : mSet(set), mValue(value) { }

        Reference &operator=(bool value) {
            mSet.set(mValue, value);
            return *this;
        }

        operator bool() {
            return mSet.test(mValue);
        }

        bool operator~() {
            return !*this;
        }

    }; // class Reference

    /** Returns a reference to the specified entry of this set. */
    Reference operator[](FileDescriptor::Value fd) noexcept {
        return Reference(*this, fd);
    }

    /** Alias to {@link #test}. */
    bool operator[](FileDescriptor::Value fd) const {
        return test(fd);
    }

}; // class FileDescriptorSet

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptorSet_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
