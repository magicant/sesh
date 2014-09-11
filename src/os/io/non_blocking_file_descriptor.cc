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

#include "buildconfig.h"
#include "non_blocking_file_descriptor.hh"

#include <utility>
#include "helpermacros.h"
#include "os/io/file_description_api.hh"
#include "os/io/file_description_attribute.hh"
#include "os/io/file_description_status.hh"
#include "os/io/file_descriptor.hh"

namespace sesh {
namespace os {
namespace io {

namespace {

std::unique_ptr<file_description_status> status_or_null(
        const file_description_api &api, const file_descriptor &fd) {
    using status_pointer = std::unique_ptr<file_description_status>;
    auto status_or_error = api.get_file_description_status(fd);
    switch (status_or_error.tag()) {
    case status_or_error.tag<status_pointer>():
        return std::move(status_or_error.value<status_pointer>());
    case status_or_error.tag<std::error_code>():
        return nullptr;
    }
    UNREACHABLE();
}

} // namespace

non_blocking_file_descriptor::non_blocking_file_descriptor(
        const file_description_api &api, file_descriptor &&fd) :
        m_api(api),
        m_file_descriptor(std::move(fd)),
        m_original_status(status_or_null(m_api, m_file_descriptor)) {
    if (m_original_status == nullptr)
        return;

    auto non_blocking_status = m_original_status->clone();
    non_blocking_status->set(file_description_attribute::non_blocking);
    m_api.set_file_description_status(m_file_descriptor, *non_blocking_status);
}

file_descriptor non_blocking_file_descriptor::release() {
    if (m_original_status != nullptr)
        m_api.set_file_description_status(
                m_file_descriptor, *m_original_status);
    return std::move(m_file_descriptor);
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
