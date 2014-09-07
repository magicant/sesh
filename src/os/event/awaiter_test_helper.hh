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

#ifndef INCLUDED_os_event_awaiter_test_helper_hh
#define INCLUDED_os_event_awaiter_test_helper_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "os/event/awaiter.hh"
#include "os/event/PselectApiTestHelper.hh"
#include "os/signaling/HandlerConfiguration.hh"

namespace sesh {
namespace os {
namespace event {

template<typename Base>
class awaiter_test_fixture : protected PselectApiStub, protected Base {

private:

    std::unique_ptr<awaiter> m_awaiter = create_awaiter(
            *this, signaling::HandlerConfiguration::create(*this));

protected:

    awaiter &a = *m_awaiter;

}; // template<typename Base> class awaiter_test_fixture

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_awaiter_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
