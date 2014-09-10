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

#ifndef INCLUDED_os_event_pselect_api_test_helper_hh
#define INCLUDED_os_event_pselect_api_test_helper_hh

#include "buildconfig.h"

#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include "os/event/pselect_api.hh"
#include "os/io/file_descriptor.hh"
#include "os/io/file_descriptor_set.hh"
#include "os/io/file_descriptor_set_test_helper.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/time_api_test_helper.hh"

/*
#include <ostream>

namespace std {

template<typename ChatT, typename Traits>
std::basic_ostream<ChatT, Traits> &operator<<(
        std::basic_ostream<ChatT, Traits> &os,
        const std::set<sesh::os::io::file_descriptor::value_type> &fds) {
    os << '{';
    for (const auto &fd : fds)
        os << fd << ',';
    return os << '}';
}

} // namespace std
*/

namespace sesh {
namespace os {
namespace event {

class pselect_api_stub : public pselect_api, public time_api_fake {

public:

    using file_descriptor_set_impl = io::file_descriptor_set_fake;

    using pselect_function = std::function<std::error_code(
            const pselect_api_stub &,
            io::file_descriptor::value_type,
            io::file_descriptor_set *,
            io::file_descriptor_set *,
            io::file_descriptor_set *,
            std::chrono::nanoseconds,
            const signaling::SignalNumberSet *)>;

private:

    mutable pselect_function m_implementation;

public:

    std::unique_ptr<io::file_descriptor_set> create_file_descriptor_set() const
            override {
        return std::unique_ptr<io::file_descriptor_set>(
                new file_descriptor_set_impl());
    }

    static void check_equal(
            const io::file_descriptor_set *actual,
            const std::set<io::file_descriptor::value_type> &expected,
            io::file_descriptor::value_type actual_bound,
            const std::string &info) {
        INFO(info);

        const file_descriptor_set_impl empty_set{};
        if (actual == nullptr)
            actual = &empty_set;

        const file_descriptor_set_impl *actual_impl =
                dynamic_cast<const file_descriptor_set_impl *>(actual);
        REQUIRE(actual_impl != nullptr);
        CHECK(*actual_impl == expected);
        CHECK(actual_impl->bound() <= actual_bound);
    }

    static void check_empty(
            const io::file_descriptor_set *fds,
            io::file_descriptor::value_type fd_bound,
            const std::string &info) {
        check_equal(
                fds,
                std::set<io::file_descriptor::value_type>{},
                fd_bound,
                info);
    }

    pselect_function &implementation() const { return m_implementation; }

    std::error_code pselect(
            io::file_descriptor::value_type fd_bound,
            io::file_descriptor_set *read_fds,
            io::file_descriptor_set *write_fds,
            io::file_descriptor_set *error_fds,
            std::chrono::nanoseconds timeout,
            const signaling::SignalNumberSet *signal_mask) const override {
        return m_implementation(
                *this,
                fd_bound,
                read_fds,
                write_fds,
                error_fds,
                timeout,
                signal_mask);
    }

}; // class pselect_api_stub

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_pselect_api_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
