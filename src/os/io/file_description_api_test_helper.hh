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

#ifndef INCLUDED_os_io_file_description_api_test_helper_hh
#define INCLUDED_os_io_file_description_api_test_helper_hh

#include "buildconfig.h"

#include <system_error>
#include "os/io/file_description_api.hh"

namespace sesh {
namespace os {
namespace io {

class file_description_api_dummy : public file_description_api {

    common::variant<std::unique_ptr<file_description_status>, std::error_code>
    get_file_description_status(const FileDescriptor &) const override {
        return std::error_code();
    }

    std::error_code set_file_description_status(
            const FileDescriptor &, const file_description_status &) const
            override {
        return std::error_code();
    }

}; // class file_description_api_dummy

} // namespace io
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_io_file_description_api_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
