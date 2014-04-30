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

#include <memory>
#include <new>
#include <stdexcept>
#include <system_error>
#include "common/ErrnoHelper.hh"
#include "os/Api.hh"
#include "os/capi.h"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::errnoCode;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using SignalAction = sesh::os::Api::SignalAction;

namespace sesh {
namespace os {

namespace {

void convert(const SignalAction &from, struct sesh_osapi_signal_action &to) {
    switch (from.index()) {
    case SignalAction::index<Api::Default>():
        to.type = SESH_OSAPI_SIG_DFL;
        to.handler = nullptr;
        break;
    case SignalAction::index<Api::Ignore>():
        to.type = SESH_OSAPI_SIG_IGN;
        to.handler = nullptr;
        break;
    case SignalAction::index<sesh_osapi_signal_handler *>():
        to.type = SESH_OSAPI_SIG_HANDLER;
        to.handler = from.value<sesh_osapi_signal_handler *>();
        break;
    }
}

void convert(const struct sesh_osapi_signal_action &from, SignalAction &to) {
    switch (from.type) {
    case SESH_OSAPI_SIG_DFL:
        to.emplace<Api::Default>();
        break;
    case SESH_OSAPI_SIG_IGN:
        to.emplace<Api::Ignore>();
        break;
    case SESH_OSAPI_SIG_HANDLER:
        to.emplace<sesh_osapi_signal_handler *>(from.handler);
        break;
    }
}

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