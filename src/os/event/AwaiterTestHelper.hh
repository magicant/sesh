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

#ifndef INCLUDED_os_event_AwaiterTestHelper_hh
#define INCLUDED_os_event_AwaiterTestHelper_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "os/event/Awaiter.hh"
#include "os/signaling/HandlerConfiguration.hh"
#include "os/test_helper/NowApiStub.hh"
#include "os/test_helper/PselectApiStub.hh"

namespace sesh {
namespace os {
namespace event {

template<typename Api>
class AwaiterTestFixture : protected Api {

private:

    std::unique_ptr<Awaiter> mAwaiter = createAwaiter(
            *this, signaling::HandlerConfiguration::create(*this));

protected:

    Awaiter &a = *mAwaiter;

}; // template<typename Api> class AwaiterTestFixture

class PselectAndNowApiStub :
        public test_helper::PselectApiStub, public test_helper::NowApiStub {
};

void addTriggers(std::vector<Trigger> &) {
}

template<typename Head, typename... Tail>
void addTriggers(std::vector<Trigger> &ts, Head &&head, Tail &&... tail) {
    ts.push_back(std::forward<Head>(head));
    addTriggers(ts, std::forward<Tail>(tail)...);
}

template<typename... Arg>
std::vector<Trigger> triggers(Arg &&... arg) {
    std::vector<Trigger> ts;
    addTriggers(ts, std::forward<Arg>(arg)...);
    return ts;
}

} // namespace event
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_event_AwaiterTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
