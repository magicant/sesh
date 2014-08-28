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
#include "Api.hh"

#include <chrono>
#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <system_error>
#include "common/ErrnoHelper.hh"
#include "common/TypeTag.hh"
#include "common/Variant.hh"
#include "common/enum_iterator.hh"
#include "common/enum_set.hh"
#include "helpermacros.h"
#include "os/capi.h"
#include "os/io/FileDescriptionAccessMode.hh"
#include "os/io/FileDescriptionAttribute.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorOpenMode.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/io/FileMode.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::TypeTag;
using sesh::common::Variant;
using sesh::common::enum_set;
using sesh::common::enumerators;
using sesh::common::errnoCode;
using sesh::os::io::FileDescriptionAccessMode;
using sesh::os::io::FileDescriptionAttribute;
using sesh::os::io::FileDescriptionStatus;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorOpenMode;
using sesh::os::io::FileDescriptorSet;
using sesh::os::io::FileMode;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using SignalAction = sesh::os::Api::SignalAction;

namespace sesh {
namespace os {

namespace {

enum sesh_osapi_fcntl_file_access_mode convert(FileDescriptionAccessMode mode)
        noexcept {
    switch (mode) {
    case FileDescriptionAccessMode::EXEC:
        return SESH_OSAPI_O_EXEC;
    case FileDescriptionAccessMode::READ_ONLY:
        return SESH_OSAPI_O_RDONLY;
    case FileDescriptionAccessMode::READ_WRITE:
        return SESH_OSAPI_O_RDWR;
    case FileDescriptionAccessMode::SEARCH:
        return SESH_OSAPI_O_SEARCH;
    case FileDescriptionAccessMode::WRITE_ONLY:
        return SESH_OSAPI_O_WRONLY;
    }
    UNREACHABLE();
}

/** @return -1 if not supported. */
int toRawFlag(FileDescriptionAccessMode mode) {
    return sesh_osapi_fcntl_file_access_mode_to_raw(convert(mode));
}

FileDescriptionAccessMode convert(enum sesh_osapi_fcntl_file_access_mode mode)
        noexcept {
    switch (mode) {
    case SESH_OSAPI_O_EXEC:
        return FileDescriptionAccessMode::EXEC;
    case SESH_OSAPI_O_RDONLY:
        return FileDescriptionAccessMode::READ_ONLY;
    case SESH_OSAPI_O_RDWR:
        return FileDescriptionAccessMode::READ_WRITE;
    case SESH_OSAPI_O_SEARCH:
        return FileDescriptionAccessMode::SEARCH;
    case SESH_OSAPI_O_WRONLY:
        return FileDescriptionAccessMode::WRITE_ONLY;
    }
    UNREACHABLE();
}

enum sesh_osapi_fcntl_file_attribute convert(FileDescriptionAttribute a)
        noexcept {
    switch (a) {
    case FileDescriptionAttribute::APPEND:
        return SESH_OSAPI_O_APPEND;
    case FileDescriptionAttribute::DATA_SYNC:
        return SESH_OSAPI_O_DSYNC;
    case FileDescriptionAttribute::NON_BLOCKING:
        return SESH_OSAPI_O_NONBLOCK;
    case FileDescriptionAttribute::READ_SYNC:
        return SESH_OSAPI_O_RSYNC;
    case FileDescriptionAttribute::SYNC:
        return SESH_OSAPI_O_SYNC;
    }
    UNREACHABLE();
}

enum sesh_osapi_open_mode convert(FileDescriptorOpenMode mode) noexcept {
    switch (mode) {
    case FileDescriptorOpenMode::CLOSE_ON_EXEC:
        return SESH_OSAPI_O_CLOEXEC;
    case FileDescriptorOpenMode::CREATE:
        return SESH_OSAPI_O_CREAT;
    case FileDescriptorOpenMode::DIRECTORY:
        return SESH_OSAPI_O_DIRECTORY;
    case FileDescriptorOpenMode::EXCLUSIVE:
        return SESH_OSAPI_O_EXCL;
    case FileDescriptorOpenMode::NO_CONTROLLING_TERMINAL:
        return SESH_OSAPI_O_NOCTTY;
    case FileDescriptorOpenMode::NO_FOLLOW:
        return SESH_OSAPI_O_NOFOLLOW;
    case FileDescriptorOpenMode::TRUNCATE:
        return SESH_OSAPI_O_TRUNC;
    case FileDescriptorOpenMode::TTY_INITIALIZE:
        return SESH_OSAPI_O_TTY_INIT;
    }
    UNREACHABLE();
}

int toRawFlag(FileDescriptionAttribute a) {
    return sesh_osapi_fcntl_file_attribute_to_raw(convert(a));
}

int toRawFlag(FileDescriptorOpenMode mode) {
    return sesh_osapi_open_mode_to_raw(convert(mode));
}

/** @return -1 if the access mode is not supported. */
int toRawFlags(
        FileDescriptionAccessMode accessMode,
        enum_set<FileDescriptionAttribute> attributes,
        enum_set<FileDescriptorOpenMode> openModes) {
    int rawFlags = toRawFlag(accessMode);
    if (rawFlags == -1)
        return -1;

    for (auto a : enumerators<FileDescriptionAttribute>())
        if (attributes[a])
            rawFlags |= toRawFlag(a);

    for (auto m : enumerators<FileDescriptorOpenMode>())
        if (openModes[m])
            rawFlags |= toRawFlag(m);

    return rawFlags;
}

int toRawModes(enum_set<FileMode> modes) {
    return sesh_osapi_mode_to_raw(static_cast<int>(modes.to_ulong()));
}

void convert(const SignalAction &from, struct sesh_osapi_signal_action &to) {
    switch (from.tag()) {
    case SignalAction::tag<Api::Default>():
        to.type = SESH_OSAPI_SIG_DFL;
        to.handler = nullptr;
        break;
    case SignalAction::tag<Api::Ignore>():
        to.type = SESH_OSAPI_SIG_IGN;
        to.handler = nullptr;
        break;
    case SignalAction::tag<sesh_osapi_signal_handler *>():
        to.type = SESH_OSAPI_SIG_HANDLER;
        to.handler = from.value<sesh_osapi_signal_handler *>();
        break;
    }
}

void convert(const struct sesh_osapi_signal_action &from, SignalAction &to) {
    switch (from.type) {
    case SESH_OSAPI_SIG_DFL:
        to.emplace(TypeTag<Api::Default>());
        break;
    case SESH_OSAPI_SIG_IGN:
        to.emplace(TypeTag<Api::Ignore>());
        break;
    case SESH_OSAPI_SIG_HANDLER:
        to.emplace(TypeTag<sesh_osapi_signal_handler *>(), from.handler);
        break;
    }
}

class FileDescriptionStatusImpl : public FileDescriptionStatus {

private:

    int mRawFlags;

public:

    explicit FileDescriptionStatusImpl(int rawFlags) noexcept :
            mRawFlags(rawFlags) { }

    int rawFlags() const noexcept { return mRawFlags; }

    FileDescriptionAccessMode accessMode() const noexcept final override {
        return convert(sesh_osapi_fcntl_file_access_mode_from_raw(mRawFlags));
    }

    bool test(FileDescriptionAttribute a) const noexcept final override {
        return mRawFlags & toRawFlag(a);
    }

    FileDescriptionStatus &set(FileDescriptionAttribute a, bool value)
            noexcept final override {
        int flag = toRawFlag(a);
        if (value)
            mRawFlags |= flag;
        else
            mRawFlags &= ~flag;
        return *this;
    }

    FileDescriptionStatus &resetAttributes() noexcept final override {
        mRawFlags &=
                sesh_osapi_fcntl_file_attribute_to_raw(SESH_OSAPI_O_ACCMODE);
        return *this;
    }

    std::unique_ptr<FileDescriptionStatus> clone() const final override {
        return std::unique_ptr<FileDescriptionStatus>(new auto(*this));
    }

}; // class FileDescriptionStatusImpl

class FileDescriptorSetImpl : public FileDescriptorSet {

private:

    /** May be null when empty. */
    std::unique_ptr<struct sesh_osapi_fd_set> mSet;

    void allocateIfNull() {
        if (mSet != nullptr)
            return;
        mSet.reset(sesh_osapi_fd_set_new());
        if (mSet == nullptr)
            throw std::bad_alloc();
        sesh_osapi_fd_zero(mSet.get());
    }

    void throwIfOutOfDomain(FileDescriptor::Value fd) {
        if (fd >= sesh_osapi_fd_setsize())
            throw std::domain_error("too large file descriptor");
    }

    void setImpl(FileDescriptor::Value fd) {
        throwIfOutOfDomain(fd);
        allocateIfNull();
        sesh_osapi_fd_set(fd, mSet.get());
    }

    void resetImpl(FileDescriptor::Value fd) {
        throwIfOutOfDomain(fd);
        if (mSet != nullptr)
            sesh_osapi_fd_clr(fd, mSet.get());
    }

public:

    FileDescriptor::Value maxValue() const override {
        return sesh_osapi_fd_setsize() - 1;
    }

    /** @return may be null. */
    struct sesh_osapi_fd_set *get() const {
        return mSet.get();
    }

    bool test(FileDescriptor::Value fd) const override {
        return fd < sesh_osapi_fd_setsize() &&
                mSet != nullptr && sesh_osapi_fd_isset(fd, mSet.get());
    }

    FileDescriptorSet &set(FileDescriptor::Value fd, bool value) override {
        if (value)
            setImpl(fd);
        else
            resetImpl(fd);
        return *this;
    }

    FileDescriptorSet &reset() override {
        if (mSet != nullptr)
            sesh_osapi_fd_zero(mSet.get());
        return *this;
    }

}; // class FileDescriptorSetImpl

class SignalNumberSetImpl : public SignalNumberSet {

private:

    /** Never null */
    std::unique_ptr<struct sesh_osapi_sigset> mSet;

public:

    SignalNumberSetImpl() : SignalNumberSet(), mSet(sesh_osapi_sigset_new()) {
        if (mSet == nullptr)
            throw std::bad_alloc();
        sesh_osapi_sigemptyset(mSet.get());
    }

    SignalNumberSetImpl(const SignalNumberSetImpl &other) :
            SignalNumberSet(other), mSet(sesh_osapi_sigset_new()) {
        if (mSet == nullptr)
            throw std::bad_alloc();
        sesh_osapi_sigcopyset(mSet.get(), other.get());
    }

    SignalNumberSetImpl(SignalNumberSetImpl &&) = default;
    SignalNumberSetImpl &operator=(const SignalNumberSetImpl &) = delete;
    SignalNumberSetImpl &operator=(SignalNumberSetImpl &&) = default;
    ~SignalNumberSetImpl() = default;

    /** @return Never null. */
    struct sesh_osapi_sigset *get() const {
        return mSet.get();
    }

    bool test(SignalNumber n) const override {
        return sesh_osapi_sigismember(mSet.get(), n);
    }

    SignalNumberSet &set(SignalNumber n, bool value) override {
        if (value)
            sesh_osapi_sigaddset(mSet.get(), n);
        else
            sesh_osapi_sigdelset(mSet.get(), n);
        return *this;
    }

    SignalNumberSet &set() override {
        sesh_osapi_sigfillset(mSet.get());
        return *this;
    }

    SignalNumberSet &reset() override {
        sesh_osapi_sigemptyset(mSet.get());
        return *this;
    }

    std::unique_ptr<SignalNumberSet> clone() const override {
        return std::unique_ptr<SignalNumberSet>(new auto(*this));
    }

}; // class SignalNumberSetImpl

class ApiImpl : public Api {

    SystemClockTime systemClockNow() const noexcept final override {
        return std::chrono::time_point_cast<SystemClockTime::duration>(
                SystemClockTime::clock::now());
    }

    SteadyClockTime steadyClockNow() const noexcept final override {
        return std::chrono::time_point_cast<SteadyClockTime::duration>(
                SteadyClockTime::clock::now());
    }

    auto getFileDescriptionStatus(const FileDescriptor &fd) const
            -> Variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
            final override {
        int flags = sesh_osapi_fcntl_getfl(fd.value());
        if (flags == -1)
            return errnoCode();
        return std::unique_ptr<FileDescriptionStatus>(
                new FileDescriptionStatusImpl(flags));
    }

    std::error_code setFileDescriptionStatus(
            const FileDescriptor &fd, const FileDescriptionStatus &s) const
            final override {
        const auto &i = static_cast<const FileDescriptionStatusImpl &>(s);
        if (sesh_osapi_fcntl_setfl(fd.value(), i.rawFlags()) == -1)
            return errnoCode();
        return std::error_code();
    }

    Variant<FileDescriptor, std::error_code> open(
            const char *path,
            FileDescriptionAccessMode accessMode,
            enum_set<FileDescriptionAttribute> attributes,
            enum_set<FileDescriptorOpenMode> openModes,
            enum_set<FileMode> fileModes) const final override {
        int flags = toRawFlags(accessMode, attributes, openModes);
        if (flags == -1)
            return std::make_error_code(std::errc::invalid_argument);

        int modes = toRawModes(fileModes);
        int fd = sesh_osapi_open(path, flags, modes);
        if (fd < 0)
            return errnoCode();
        return FileDescriptor(fd);
    }

    std::error_code close(FileDescriptor &fd) const final override {
        if (sesh_osapi_close(fd.value()) == 0) {
            fd.clear();
            return std::error_code();
        }

        std::error_code ec = errnoCode();
        if (ec == std::errc::bad_file_descriptor)
            fd.clear();
        return ec;
    }

    ReadResult read(
            const FileDescriptor &fd, void *buffer, std::size_t maxBytesToRead)
            const final override {
        std::size_t bytesRead =
                sesh_osapi_read(fd.value(), buffer, maxBytesToRead);
        if (std::error_code ec = errnoCode())
            return ec;
        return bytesRead;
    }

    WriteResult write(
            const FileDescriptor &fd,
            const void *bytes,
            std::size_t bytesToWrite)
            const final override {
        std::size_t bytesWritten =
                sesh_osapi_write(fd.value(), bytes, bytesToWrite);
        if (std::error_code ec = errnoCode())
            return ec;
        return bytesWritten;
    }

    std::unique_ptr<FileDescriptorSet> createFileDescriptorSet() const
            final override {
        std::unique_ptr<FileDescriptorSet> set(new FileDescriptorSetImpl);
        return set;
    }

    std::unique_ptr<SignalNumberSet> createSignalNumberSet() const
            final override {
        std::unique_ptr<SignalNumberSet> set(new SignalNumberSetImpl);
        return set;
    }

    std::error_code pselect(
                FileDescriptor::Value fdBound,
                FileDescriptorSet *readFds,
                FileDescriptorSet *writeFds,
                FileDescriptorSet *errorFds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signalMask) const final override {
        FileDescriptorSetImpl
                *readFdsImpl = static_cast<FileDescriptorSetImpl *>(readFds),
                *writeFdsImpl = static_cast<FileDescriptorSetImpl *>(writeFds),
                *errorFdsImpl = static_cast<FileDescriptorSetImpl *>(errorFds);
        const SignalNumberSetImpl *signalMaskImpl =
                static_cast<const SignalNumberSetImpl *>(signalMask);

        int pselectResult = sesh_osapi_pselect(
                fdBound,
                readFds == nullptr ? nullptr : readFdsImpl->get(),
                writeFds == nullptr ? nullptr : writeFdsImpl->get(),
                errorFds == nullptr ? nullptr : errorFdsImpl->get(),
                timeout.count(),
                signalMaskImpl == nullptr ? nullptr : signalMaskImpl->get());
        if (pselectResult == 0)
            return std::error_code();
        return errnoCode();
    }

    std::error_code sigprocmask(
            MaskChangeHow how,
            const signaling::SignalNumberSet *newMask,
            signaling::SignalNumberSet *oldMask) const final override {
        enum sesh_osapi_sigprocmask_how howImpl;

        switch (how) {
        case MaskChangeHow::BLOCK:
            howImpl = SESH_OSAPI_SIG_BLOCK;
            break;
        case MaskChangeHow::UNBLOCK:
            howImpl = SESH_OSAPI_SIG_UNBLOCK;
            break;
        case MaskChangeHow::SET_MASK:
            howImpl = SESH_OSAPI_SIG_SETMASK;
            break;
        }

        const SignalNumberSetImpl *newMaskImpl =
                static_cast<const SignalNumberSetImpl *>(newMask);
        SignalNumberSetImpl *oldMaskImpl =
                static_cast<SignalNumberSetImpl *>(oldMask);

        int sigprocmaskResult = sesh_osapi_sigprocmask(
                howImpl,
                newMaskImpl == nullptr ? nullptr : newMaskImpl->get(),
                oldMaskImpl == nullptr ? nullptr : oldMaskImpl->get());
        if (sigprocmaskResult == 0)
            return std::error_code();
        return errnoCode();
    }

    std::error_code sigaction(
            signaling::SignalNumber n,
            const SignalAction *newAction,
            SignalAction *oldAction) const final override {
        struct sesh_osapi_signal_action newActionImpl, oldActionImpl;

        if (newAction != nullptr)
            convert(*newAction, newActionImpl);

        int sigactionResult = sesh_osapi_sigaction(
                n,
                newAction == nullptr ? nullptr : &newActionImpl,
                oldAction == nullptr ? nullptr : &oldActionImpl);

        if (sigactionResult != 0)
            return errnoCode();

        if (oldAction != nullptr)
            convert(oldActionImpl, *oldAction);

        return std::error_code();
    }

}; // class ApiImpl

} // namespace

const Api &Api::INSTANCE = ApiImpl();

} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
