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

#ifndef INCLUDED_os_test_helper_FileDescriptorSetApi_hh
#define INCLUDED_os_test_helper_FileDescriptorSetApi_hh

#include "buildconfig.h"

#include <memory>
#include <set>
#include <string>
#include "os/io/FileDescriptorSetTestHelper.hh"
#include "os/test_helper/UnimplementedApi.hh"

/*
#include <ostream>

namespace std {

template<typename ChatT, typename Traits>
std::basic_ostream<ChatT, Traits> &operator<<(
        std::basic_ostream<ChatT, Traits> &os,
        const std::set<sesh::os::io::FileDescriptor::Value> &fds) {
    os << '{';
    for (const auto &fd : fds)
        os << fd << ',';
    return os << '}';
}

} // namespace std
*/

namespace sesh {
namespace os {
namespace test_helper {

class FileDescriptorSetApi : public virtual UnimplementedApi {

public:

    using FileDescriptorSetImpl = io::FileDescriptorSetFake;

    std::unique_ptr<io::FileDescriptorSet> createFileDescriptorSet() const
            override {
        return std::unique_ptr<io::FileDescriptorSet>(
                new FileDescriptorSetImpl());
    }

    static void checkEqual(
            const io::FileDescriptorSet *actual,
            const std::set<io::FileDescriptor::Value> &expected,
            io::FileDescriptor::Value actualBound,
            const std::string &info) {
        INFO(info);

        const FileDescriptorSetImpl emptySet{};
        if (actual == nullptr)
            actual = &emptySet;

        const FileDescriptorSetImpl *actualImpl =
                dynamic_cast<const FileDescriptorSetImpl *>(actual);
        REQUIRE(actualImpl != nullptr);
        CHECK(*actualImpl == expected);
        CHECK(actualImpl->bound() <= actualBound);
    }

    static void checkEmpty(
            const io::FileDescriptorSet *fds,
            io::FileDescriptor::Value fdBound,
            const std::string &info) {
        checkEqual(fds, std::set<io::FileDescriptor::Value>{}, fdBound, info);
    }

}; // class FileDescriptorSetApi

} // namespace test_helper
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_test_helper_FileDescriptorSetApi_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
