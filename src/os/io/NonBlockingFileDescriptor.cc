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
#include "common/Variant.hh"
#include "helpermacros.h"
#include "os/io/FileDescriptionApi.hh"
#include "os/io/FileDescriptionAttribute.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"

using sesh::common::Variant;

namespace sesh {
namespace os {
namespace io {

namespace {

std::unique_ptr<FileDescriptionStatus> statusOrNull(
        const FileDescriptionApi &api, const FileDescriptor &fd) {
    using StatusPointer = std::unique_ptr<FileDescriptionStatus>;
    auto statusOrError = api.getFileDescriptionStatus(fd);
    switch (statusOrError.index()) {
    case statusOrError.index<StatusPointer>():
        return std::move(statusOrError.value<StatusPointer>());
    case statusOrError.index<std::error_code>():
        return nullptr;
    }
    UNREACHABLE();
}

} // namespace

NonBlockingFileDescriptor::NonBlockingFileDescriptor(
        const FileDescriptionApi &api, FileDescriptor &&fd) :
        mApi(api),
        mFileDescriptor(std::move(fd)),
        mOriginalStatus(statusOrNull(mApi, mFileDescriptor)) {
    if (mOriginalStatus == nullptr)
        return;

    auto nonBlockingStatus = mOriginalStatus->clone();
    nonBlockingStatus->set(FileDescriptionAttribute::NON_BLOCKING);
    mApi.setFileDescriptionStatus(mFileDescriptor, *nonBlockingStatus);
}

FileDescriptor NonBlockingFileDescriptor::release() {
    if (mOriginalStatus != nullptr)
        mApi.setFileDescriptionStatus(mFileDescriptor, *mOriginalStatus);
    return std::move(mFileDescriptor);
}

} // namespace io
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
