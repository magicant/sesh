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
#include "api.hh"

#include <chrono>
#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <system_error>
#include "common/enum_iterator.hh"
#include "common/enum_set.hh"
#include "common/errno_helper.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"
#include "helpermacros.h"
#include "os/capi.h"
#include "os/io/file_description_access_mode.hh"
#include "os/io/FileDescriptionAttribute.hh"
#include "os/io/FileDescriptionStatus.hh"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorOpenMode.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/io/FileMode.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::enum_set;
using sesh::common::enumerators;
using sesh::common::errno_code;
using sesh::common::type_tag;
using sesh::common::variant;
using sesh::os::io::file_description_access_mode;
using sesh::os::io::FileDescriptionAttribute;
using sesh::os::io::FileDescriptionStatus;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorOpenMode;
using sesh::os::io::FileDescriptorSet;
using sesh::os::io::FileMode;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using SignalAction = sesh::os::api::SignalAction;

namespace sesh {
namespace os {

namespace {

enum sesh_osapi_fcntl_file_access_mode convert(
        file_description_access_mode mode) noexcept {
    switch (mode) {
    case file_description_access_mode::exec:
        return SESH_OSAPI_O_EXEC;
    case file_description_access_mode::read_only:
        return SESH_OSAPI_O_RDONLY;
    case file_description_access_mode::read_write:
        return SESH_OSAPI_O_RDWR;
    case file_description_access_mode::search:
        return SESH_OSAPI_O_SEARCH;
    case file_description_access_mode::write_only:
        return SESH_OSAPI_O_WRONLY;
    }
    UNREACHABLE();
}

/** @return -1 if not supported. */
int to_raw_flag(file_description_access_mode mode) {
    return sesh_osapi_fcntl_file_access_mode_to_raw(convert(mode));
}

file_description_access_mode convert(
        enum sesh_osapi_fcntl_file_access_mode mode) noexcept {
    switch (mode) {
    case SESH_OSAPI_O_EXEC:
        return file_description_access_mode::exec;
    case SESH_OSAPI_O_RDONLY:
        return file_description_access_mode::read_only;
    case SESH_OSAPI_O_RDWR:
        return file_description_access_mode::read_write;
    case SESH_OSAPI_O_SEARCH:
        return file_description_access_mode::search;
    case SESH_OSAPI_O_WRONLY:
        return file_description_access_mode::write_only;
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

int to_raw_flag(FileDescriptionAttribute a) {
    return sesh_osapi_fcntl_file_attribute_to_raw(convert(a));
}

int to_raw_flag(FileDescriptorOpenMode mode) {
    return sesh_osapi_open_mode_to_raw(convert(mode));
}

/** @return -1 if the access mode is not supported. */
int to_raw_flags(
        file_description_access_mode access_mode,
        enum_set<FileDescriptionAttribute> attributes,
        enum_set<FileDescriptorOpenMode> open_modes) {
    int raw_flags = to_raw_flag(access_mode);
    if (raw_flags == -1)
        return -1;

    for (auto a : enumerators<FileDescriptionAttribute>())
        if (attributes[a])
            raw_flags |= to_raw_flag(a);

    for (auto m : enumerators<FileDescriptorOpenMode>())
        if (open_modes[m])
            raw_flags |= to_raw_flag(m);

    return raw_flags;
}

int to_raw_modes(enum_set<FileMode> modes) {
    return sesh_osapi_mode_to_raw(static_cast<int>(modes.to_ulong()));
}

void convert(const SignalAction &from, struct sesh_osapi_signal_action &to) {
    switch (from.tag()) {
    case SignalAction::tag<api::Default>():
        to.type = SESH_OSAPI_SIG_DFL;
        to.handler = nullptr;
        break;
    case SignalAction::tag<api::Ignore>():
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
        to.emplace(type_tag<api::Default>());
        break;
    case SESH_OSAPI_SIG_IGN:
        to.emplace(type_tag<api::Ignore>());
        break;
    case SESH_OSAPI_SIG_HANDLER:
        to.emplace(type_tag<sesh_osapi_signal_handler *>(), from.handler);
        break;
    }
}

class file_description_status_impl : public FileDescriptionStatus {

private:

    int m_raw_flags;

public:

    explicit file_description_status_impl(int raw_flags) noexcept :
            m_raw_flags(raw_flags) { }

    int raw_flags() const noexcept { return m_raw_flags; }

    file_description_access_mode accessMode() const noexcept final override {
        return convert(
                sesh_osapi_fcntl_file_access_mode_from_raw(m_raw_flags));
    }

    bool test(FileDescriptionAttribute a) const noexcept final override {
        return m_raw_flags & to_raw_flag(a);
    }

    FileDescriptionStatus &set(FileDescriptionAttribute a, bool value)
            noexcept final override {
        int flag = to_raw_flag(a);
        if (value)
            m_raw_flags |= flag;
        else
            m_raw_flags &= ~flag;
        return *this;
    }

    FileDescriptionStatus &resetAttributes() noexcept final override {
        m_raw_flags &=
                sesh_osapi_fcntl_file_attribute_to_raw(SESH_OSAPI_O_ACCMODE);
        return *this;
    }

    std::unique_ptr<FileDescriptionStatus> clone() const final override {
        return std::unique_ptr<FileDescriptionStatus>(new auto(*this));
    }

}; // class file_description_status_impl

class file_descriptor_set_impl : public FileDescriptorSet {

private:

    /** May be null when empty. */
    std::unique_ptr<struct sesh_osapi_fd_set> m_set;

    void allocate_if_null() {
        if (m_set != nullptr)
            return;
        m_set.reset(sesh_osapi_fd_set_new());
        if (m_set == nullptr)
            throw std::bad_alloc();
        sesh_osapi_fd_zero(m_set.get());
    }

    void throw_if_out_of_domain(FileDescriptor::Value fd) {
        if (fd >= sesh_osapi_fd_setsize())
            throw std::domain_error("too large file descriptor");
    }

    void set_impl(FileDescriptor::Value fd) {
        throw_if_out_of_domain(fd);
        allocate_if_null();
        sesh_osapi_fd_set(fd, m_set.get());
    }

    void reset_impl(FileDescriptor::Value fd) {
        throw_if_out_of_domain(fd);
        if (m_set != nullptr)
            sesh_osapi_fd_clr(fd, m_set.get());
    }

public:

    FileDescriptor::Value maxValue() const override {
        return sesh_osapi_fd_setsize() - 1;
    }

    /** @return may be null. */
    struct sesh_osapi_fd_set *get() const {
        return m_set.get();
    }

    bool test(FileDescriptor::Value fd) const override {
        return fd < sesh_osapi_fd_setsize() &&
                m_set != nullptr && sesh_osapi_fd_isset(fd, m_set.get());
    }

    FileDescriptorSet &set(FileDescriptor::Value fd, bool value) override {
        if (value)
            set_impl(fd);
        else
            reset_impl(fd);
        return *this;
    }

    FileDescriptorSet &reset() override {
        if (m_set != nullptr)
            sesh_osapi_fd_zero(m_set.get());
        return *this;
    }

}; // class file_descriptor_set_impl

class signal_number_set_impl : public SignalNumberSet {

private:

    /** Never null */
    std::unique_ptr<struct sesh_osapi_sigset> m_set;

public:

    signal_number_set_impl() :
            SignalNumberSet(), m_set(sesh_osapi_sigset_new()) {
        if (m_set == nullptr)
            throw std::bad_alloc();
        sesh_osapi_sigemptyset(m_set.get());
    }

    signal_number_set_impl(const signal_number_set_impl &other) :
            SignalNumberSet(other), m_set(sesh_osapi_sigset_new()) {
        if (m_set == nullptr)
            throw std::bad_alloc();
        sesh_osapi_sigcopyset(m_set.get(), other.get());
    }

    signal_number_set_impl(signal_number_set_impl &&) = default;
    signal_number_set_impl &operator=(const signal_number_set_impl &) = delete;
    signal_number_set_impl &operator=(signal_number_set_impl &&) = default;
    ~signal_number_set_impl() = default;

    /** @return Never null. */
    struct sesh_osapi_sigset *get() const {
        return m_set.get();
    }

    bool test(SignalNumber n) const override {
        return sesh_osapi_sigismember(m_set.get(), n);
    }

    SignalNumberSet &set(SignalNumber n, bool value) override {
        if (value)
            sesh_osapi_sigaddset(m_set.get(), n);
        else
            sesh_osapi_sigdelset(m_set.get(), n);
        return *this;
    }

    SignalNumberSet &set() override {
        sesh_osapi_sigfillset(m_set.get());
        return *this;
    }

    SignalNumberSet &reset() override {
        sesh_osapi_sigemptyset(m_set.get());
        return *this;
    }

    std::unique_ptr<SignalNumberSet> clone() const override {
        return std::unique_ptr<SignalNumberSet>(new auto(*this));
    }

}; // class signal_number_set_impl

class api_impl : public api {

    system_clock_time system_clock_now() const noexcept final override {
        return std::chrono::time_point_cast<system_clock_time::duration>(
                system_clock_time::clock::now());
    }

    steady_clock_time steady_clock_now() const noexcept final override {
        return std::chrono::time_point_cast<steady_clock_time::duration>(
                steady_clock_time::clock::now());
    }

    auto get_file_description_status(const FileDescriptor &fd) const
            -> variant<std::unique_ptr<FileDescriptionStatus>, std::error_code>
            final override {
        int flags = sesh_osapi_fcntl_getfl(fd.value());
        if (flags == -1)
            return errno_code();
        return std::unique_ptr<FileDescriptionStatus>(
                new file_description_status_impl(flags));
    }

    std::error_code set_file_description_status(
            const FileDescriptor &fd, const FileDescriptionStatus &s) const
            final override {
        const auto &i = static_cast<const file_description_status_impl &>(s);
        if (sesh_osapi_fcntl_setfl(fd.value(), i.raw_flags()) == -1)
            return errno_code();
        return std::error_code();
    }

    variant<FileDescriptor, std::error_code> open(
            const char *path,
            file_description_access_mode access_mode,
            enum_set<FileDescriptionAttribute> attributes,
            enum_set<FileDescriptorOpenMode> open_modes,
            enum_set<FileMode> file_modes) const final override {
        int flags = to_raw_flags(access_mode, attributes, open_modes);
        if (flags == -1)
            return std::make_error_code(std::errc::invalid_argument);

        int modes = to_raw_modes(file_modes);
        int fd = sesh_osapi_open(path, flags, modes);
        if (fd < 0)
            return errno_code();
        return FileDescriptor(fd);
    }

    std::error_code close(FileDescriptor &fd) const final override {
        if (sesh_osapi_close(fd.value()) == 0) {
            fd.clear();
            return std::error_code();
        }

        std::error_code ec = errno_code();
        if (ec == std::errc::bad_file_descriptor)
            fd.clear();
        return ec;
    }

    ReadResult read(
            const FileDescriptor &fd,
            void *buffer,
            std::size_t max_bytes_to_read)
            const final override {
        std::size_t bytes_read =
                sesh_osapi_read(fd.value(), buffer, max_bytes_to_read);
        if (std::error_code ec = errno_code())
            return ec;
        return bytes_read;
    }

    WriteResult write(
            const FileDescriptor &fd,
            const void *bytes,
            std::size_t bytes_to_write)
            const final override {
        std::size_t bytes_written =
                sesh_osapi_write(fd.value(), bytes, bytes_to_write);
        if (std::error_code ec = errno_code())
            return ec;
        return bytes_written;
    }

    std::unique_ptr<FileDescriptorSet> create_file_descriptor_set() const
            final override {
        std::unique_ptr<FileDescriptorSet> set(new file_descriptor_set_impl);
        return set;
    }

    std::unique_ptr<SignalNumberSet> createSignalNumberSet() const
            final override {
        std::unique_ptr<SignalNumberSet> set(new signal_number_set_impl);
        return set;
    }

    std::error_code pselect(
                FileDescriptor::Value fd_bound,
                FileDescriptorSet *read_fds,
                FileDescriptorSet *write_fds,
                FileDescriptorSet *error_fds,
                std::chrono::nanoseconds timeout,
                const SignalNumberSet *signal_mask) const final override {
        file_descriptor_set_impl *read_fds_impl =
                static_cast<file_descriptor_set_impl *>(read_fds);
        file_descriptor_set_impl *write_fds_impl =
                static_cast<file_descriptor_set_impl *>(write_fds);
        file_descriptor_set_impl *error_fds_impl =
                static_cast<file_descriptor_set_impl *>(error_fds);
        const signal_number_set_impl *signal_mask_impl =
                static_cast<const signal_number_set_impl *>(signal_mask);

        int pselect_result = sesh_osapi_pselect(
                fd_bound,
                read_fds == nullptr ? nullptr : read_fds_impl->get(),
                write_fds == nullptr ? nullptr : write_fds_impl->get(),
                error_fds == nullptr ? nullptr : error_fds_impl->get(),
                timeout.count(),
                signal_mask == nullptr ? nullptr : signal_mask_impl->get());
        if (pselect_result == 0)
            return std::error_code();
        return errno_code();
    }

    std::error_code sigprocmask(
            MaskChangeHow how,
            const signaling::SignalNumberSet *new_mask,
            signaling::SignalNumberSet *old_mask) const final override {
        enum sesh_osapi_sigprocmask_how how_impl;

        switch (how) {
        case MaskChangeHow::BLOCK:
            how_impl = SESH_OSAPI_SIG_BLOCK;
            break;
        case MaskChangeHow::UNBLOCK:
            how_impl = SESH_OSAPI_SIG_UNBLOCK;
            break;
        case MaskChangeHow::SET_MASK:
            how_impl = SESH_OSAPI_SIG_SETMASK;
            break;
        }

        const signal_number_set_impl *new_mask_impl =
                static_cast<const signal_number_set_impl *>(new_mask);
        signal_number_set_impl *old_mask_impl =
                static_cast<signal_number_set_impl *>(old_mask);

        int sigprocmask_result = sesh_osapi_sigprocmask(
                how_impl,
                new_mask_impl == nullptr ? nullptr : new_mask_impl->get(),
                old_mask_impl == nullptr ? nullptr : old_mask_impl->get());
        if (sigprocmask_result == 0)
            return std::error_code();
        return errno_code();
    }

    std::error_code sigaction(
            signaling::SignalNumber n,
            const SignalAction *new_action,
            SignalAction *old_action) const final override {
        struct sesh_osapi_signal_action new_action_impl, old_action_impl;

        if (new_action != nullptr)
            convert(*new_action, new_action_impl);

        int sigaction_result = sesh_osapi_sigaction(
                n,
                new_action == nullptr ? nullptr : &new_action_impl,
                old_action == nullptr ? nullptr : &old_action_impl);

        if (sigaction_result != 0)
            return errno_code();

        if (old_action != nullptr)
            convert(old_action_impl, *old_action);

        return std::error_code();
    }

}; // class api_impl

} // namespace

const api &api::instance = api_impl();

} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
