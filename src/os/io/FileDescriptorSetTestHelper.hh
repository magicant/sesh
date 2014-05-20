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

#ifndef INCLUDED_os_io_FileDescriptorSetTestHelper_hh
#define INCLUDED_os_io_FileDescriptorSetTestHelper_hh

#include "buildconfig.h"

#include <set>
#include <stdexcept>
#include "common/ContainerHelper.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"

namespace sesh {
namespace os {
namespace io {

class FileDescriptorSetFake :
        public FileDescriptorSet, public std::set<FileDescriptor::Value> {

public:

    constexpr static FileDescriptor::Value MAX_VALUE = 20;

    FileDescriptor::Value maxValue() const override {
        return MAX_VALUE;
    }

    bool test(FileDescriptor::Value fd) const override {
        return common::contains(*this, fd);
    }

    std::pair<iterator, bool> insert(FileDescriptor::Value fd) {
        if (fd >= MAX_VALUE)
            throw std::domain_error("too large file descriptor");
        return std::set<FileDescriptor::Value>::insert(fd);
    }

    FileDescriptorSet &set(FileDescriptor::Value fd, bool v = true) override {
        if (v)
            insert(fd);
        else
            erase(fd);
        return *this;
    }

    FileDescriptorSet &reset() override {
        clear();
        return *this;
    }

}; // class FileDescriptorSetFake

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_FileDescriptorSetTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
