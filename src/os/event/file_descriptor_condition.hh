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

#ifndef INCLUDED_os_event_file_descriptor_condition_hh
#define INCLUDED_os_event_file_descriptor_condition_hh

#include "buildconfig.h"

#include "os/io/file_descriptor.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * Represents an event triggered by a specific condition of a file descriptor.
 */
class file_descriptor_condition {

private:

    io::file_descriptor::value_type m_value;

public:

    constexpr explicit file_descriptor_condition(
            io::file_descriptor::value_type fd) noexcept :
            m_value(fd) { }

    file_descriptor_condition(const io::file_descriptor &fd) noexcept :
            file_descriptor_condition(fd.value()) { }

    file_descriptor_condition(const io::file_descriptor &&) = delete;

    constexpr io::file_descriptor::value_type value() const noexcept {
        return m_value;
    }

}; // class file_descriptor_condition

constexpr inline bool operator==(
        const file_descriptor_condition &l, const file_descriptor_condition &r)
        noexcept {
    return l.value() == r.value();
}

constexpr inline bool operator<(
        const file_descriptor_condition &l, const file_descriptor_condition &r)
        noexcept {
    return l.value() < r.value();
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_file_descriptor_condition_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
