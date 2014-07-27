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

#ifndef INCLUDED_os_Api_hh
#define INCLUDED_os_Api_hh

#include "buildconfig.h"

#include "os/event/PselectApi.hh"
#include "os/io/FileDescriptionApi.hh"
#include "os/io/FileDescriptorApi.hh"
#include "os/signaling/HandlerConfigurationApi.hh"

namespace sesh {
namespace os {

/** Abstraction of POSIX API. */
class Api :
        public event::PselectApi,
        public io::FileDescriptionApi,
        public io::FileDescriptorApi,
        public signaling::HandlerConfigurationApi {

public:

    /** Reference to the only instance of real API implementation. */
    static const Api &INSTANCE;

}; // class Api

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_Api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
