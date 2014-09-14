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
#include "handler_configuration_api.hh"

#include "os/signaling/signal_number.hh"
#include "os/signaling/signal_number_set.hh"

namespace sesh {
namespace os {
namespace signaling {

std::error_code handler_configuration_api::sigprocmask_block(signal_number n)
        const {
    std::unique_ptr<signal_number_set> set = create_signal_number_set();
    set->set(n);
    return sigprocmask(mask_change_how::block, set.get(), nullptr);
}

std::error_code handler_configuration_api::sigprocmask_unblock(signal_number n)
        const {
    std::unique_ptr<signal_number_set> set = create_signal_number_set();
    set->set(n);
    return sigprocmask(mask_change_how::unblock, set.get(), nullptr);
}

} // namespace signaling
} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
