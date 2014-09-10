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
#include "NonBlockingFileDescriptor.hh"

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

std::unique_ptr<file_description_status> statusOrNull(
        const file_description_api &api, const file_descriptor &fd) {
    using StatusPointer = std::unique_ptr<file_description_status>;
    auto statusOrError = api.get_file_description_status(fd);
    switch (statusOrError.tag()) {
    case statusOrError.tag<StatusPointer>():
        return std::move(statusOrError.value<StatusPointer>());
    case statusOrError.tag<std::error_code>():
        return nullptr;
    }
    UNREACHABLE();
}

} // namespace

NonBlockingFileDescriptor::NonBlockingFileDescriptor(
        const file_description_api &api, file_descriptor &&fd) :
        mApi(api),
        mFileDescriptor(std::move(fd)),
        mOriginalStatus(statusOrNull(mApi, mFileDescriptor)) {
    if (mOriginalStatus == nullptr)
        return;

    auto nonBlockingStatus = mOriginalStatus->clone();
    nonBlockingStatus->set(file_description_attribute::non_blocking);
    mApi.set_file_description_status(mFileDescriptor, *nonBlockingStatus);
}

file_descriptor NonBlockingFileDescriptor::release() {
    if (mOriginalStatus != nullptr)
        mApi.set_file_description_status(mFileDescriptor, *mOriginalStatus);
    return std::move(mFileDescriptor);
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
