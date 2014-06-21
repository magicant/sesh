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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>
#include <exception>
#include "common/Try.hh"
#include "os/event/AwaiterTestHelper.hh"
#include "os/event/ReadableFileDescriptor.hh"
#include "os/event/Timeout.hh"
#include "os/event/Trigger.hh"

namespace {

using sesh::common::Try;
using sesh::os::event::AwaiterTestFixture;
using sesh::os::event::PselectAndNowApiStub;
using sesh::os::event::ReadableFileDescriptor;
using sesh::os::event::Timeout;
using sesh::os::event::Trigger;

TEST_CASE_METHOD(
        AwaiterTestFixture<PselectAndNowApiStub>,
        "Awaiter: setting timeout from domain error") {
    auto fd = ReadableFileDescriptor(FileDescriptorSetImpl::MAX_VALUE + 1);
    bool called = false;
    a.expect(fd).wrap().recover([this](std::exception_ptr) {
        return a.expect(Timeout(std::chrono::seconds(0)));
    }).unwrap().setCallback([&](Try<Trigger> &&) {
        called = true;
    });
    a.awaitEvents();
    CHECK(called);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
