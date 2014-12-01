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

#ifndef INCLUDED_os_io_file_descriptor_set_test_helper_hh
#define INCLUDED_os_io_file_descriptor_set_test_helper_hh

#include "buildconfig.h"

#include <set>
#include <stdexcept>
#include "common/container_helper.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"

namespace sesh {
namespace os {
namespace io {

class file_descriptor_set_fake :
        public file_descriptor_set,
        public std::set<file_descriptor::value_type> {

public:

    constexpr static file_descriptor::value_type max = 20;

    file_descriptor::value_type max_value() const override {
        return max;
    }

    bool test(file_descriptor::value_type fd) const override {
        return common::contains(*this, fd);
    }

    std::pair<iterator, bool> insert(file_descriptor::value_type fd) {
        if (fd > max)
            throw std::domain_error("too large file descriptor");
        return std::set<file_descriptor::value_type>::insert(fd);
    }

    file_descriptor_set &set(file_descriptor::value_type fd, bool v = true)
            override {
        if (v)
            insert(fd);
        else
            erase(fd);
        return *this;
    }

    file_descriptor_set &reset() override {
        clear();
        return *this;
    }

    file_descriptor::value_type bound() const {
        return empty() ? 0 : *rbegin() + 1;
    }

}; // class file_descriptor_set_fake

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_descriptor_set_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
