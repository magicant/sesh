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

#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

namespace sesh {
namespace os {

std::error_code Api::sigprocmaskBlock(SignalNumber n) const {
    std::unique_ptr<SignalNumberSet> set = createSignalNumberSet();
    set->set(n);
    return sigprocmask(MaskChangeHow::BLOCK, set.get(), nullptr);
}

std::error_code Api::sigprocmaskUnblock(SignalNumber n) const {
    std::unique_ptr<SignalNumberSet> set = createSignalNumberSet();
    set->set(n);
    return sigprocmask(MaskChangeHow::UNBLOCK, set.get(), nullptr);
}

} // namespace os
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
