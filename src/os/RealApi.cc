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
#include "RealApi.hh"

#include <memory>
#include <new>
#include <system_error>
#include "common/ErrnoHelper.hh"
#include "os/api.h"
#include "os/io/FileDescriptor.hh"
#include "os/io/FileDescriptorSet.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::common::errnoCondition;
using sesh::os::io::FileDescriptor;
using sesh::os::io::FileDescriptorSet;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

namespace sesh {
namespace os {

namespace {

class RealFileDescriptorSet : public FileDescriptorSet {

private:

    /** May be null when empty. */
    std::unique_ptr<struct sesh_osapi_fd_set> mSet;

    void allocateIfNull() {
        if (mSet != nullptr)
            return;
        mSet.reset(sesh_osapi_fd_set_new());
        if (mSet == nullptr)
            throw std::bad_alloc();
    }

    void setImpl(FileDescriptor::Value fd) {
        allocateIfNull();
        sesh_osapi_fd_set(fd, mSet.get());
    }

    void resetImpl(FileDescriptor::Value fd) {
        if (mSet != nullptr)
            sesh_osapi_fd_clr(fd, mSet.get());
    }

public:

    /** @return may be null. */
    struct sesh_osapi_fd_set *get() const {
        return mSet.get();
    }

    bool test(FileDescriptor::Value fd) const override {
        return mSet != nullptr && sesh_osapi_fd_isset(fd, mSet.get());
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

}; // class RealFileDescriptorSet

class RealSignalNumberSet : public SignalNumberSet {

private:

    /** May be null when empty. */
    std::unique_ptr<struct sesh_osapi_sigset> mSet;

    void allocateIfNull() {
        if (mSet != nullptr)
            return;
        mSet.reset(sesh_osapi_sigset_new());
        if (mSet == nullptr)
            throw std::bad_alloc();
    }

    void setImpl(SignalNumber n) {
        allocateIfNull();
        sesh_osapi_sigaddset(mSet.get(), n);
    }

    void resetImpl(SignalNumber n) {
        if (mSet != nullptr)
            sesh_osapi_sigdelset(mSet.get(), n);
    }

public:

    /** @return may be null. */
    struct sesh_osapi_sigset *get() const {
        return mSet.get();
    }

    bool test(SignalNumber n) override {
        return mSet != nullptr && sesh_osapi_sigismember(mSet.get(), n);
    }

    SignalNumberSet &set(SignalNumber n, bool value) override {
        if (value)
            setImpl(n);
        else
            resetImpl(n);
        return *this;
    }

    SignalNumberSet &set() override {
        allocateIfNull();
        sesh_osapi_sigfillset(mSet.get());
        return *this;
    }

    SignalNumberSet &reset() override {
        if (mSet != nullptr)
            sesh_osapi_sigemptyset(mSet.get());
        return *this;
    }

}; // class RealSignalNumberSet

} // namespace

std::error_condition RealApi::close(FileDescriptor &fd) const {
    if (sesh_osapi_close(fd.value()) == 0) {
        fd.clear();
        return std::error_condition();
    }

    std::error_condition ec = errnoCondition();
    if (ec == std::make_error_condition(std::errc::bad_file_descriptor))
        fd.clear();
    return ec;
}

std::unique_ptr<FileDescriptorSet> RealApi::createFileDescriptorSet() const {
    std::unique_ptr<FileDescriptorSet> set(new RealFileDescriptorSet);
    return set;
}

std::unique_ptr<SignalNumberSet> RealApi::createSignalNumberSet() const {
    std::unique_ptr<SignalNumberSet> set(new RealSignalNumberSet);
    return set;
}

const Api &RealApi::INSTANCE = RealApi();

} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
