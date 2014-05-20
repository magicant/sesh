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

#ifndef INCLUDED_os_event_FileDescriptorCondition_hh
#define INCLUDED_os_event_FileDescriptorCondition_hh

#include "buildconfig.h"

#include "os/io/FileDescriptor.hh"

namespace sesh {
namespace os {
namespace event {

/**
 * Represents an event triggered by a specific condition of a file descriptor.
 */
class FileDescriptorCondition {

private:

    io::FileDescriptor::Value mValue;

public:

    constexpr explicit FileDescriptorCondition(io::FileDescriptor::Value fd)
            noexcept : mValue(fd) { }

    FileDescriptorCondition(const io::FileDescriptor &fd) noexcept :
            FileDescriptorCondition(fd.value()) { }

    FileDescriptorCondition(const io::FileDescriptor &&) = delete;

    constexpr io::FileDescriptor::Value value() const noexcept {
        return mValue;
    }

}; // class FileDescriptorCondition

constexpr inline bool operator==(
        const FileDescriptorCondition &l, const FileDescriptorCondition &r)
        noexcept {
    return l.value() == r.value();
}

constexpr inline bool operator<(
        const FileDescriptorCondition &l, const FileDescriptorCondition &r)
        noexcept {
    return l.value() < r.value();
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_FileDescriptorCondition_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
