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

#ifndef INCLUDED_os_api_hh
#define INCLUDED_os_api_hh

#include "buildconfig.h"

#include "os/event/pselect_api.hh"
#include "os/io/file_description_api.hh"
#include "os/io/file_descriptor_api.hh"
#include "os/io/reader_api.hh"
#include "os/io/WriterApi.hh"
#include "os/signaling/HandlerConfigurationApi.hh"

namespace sesh {
namespace os {

/** Abstraction of POSIX API. */
class api :
        public event::pselect_api,
        public io::file_description_api,
        public io::file_descriptor_api,
        public io::reader_api,
        public io::WriterApi,
        public signaling::HandlerConfigurationApi {

public:

    /** Reference to the only instance of real API implementation. */
    static const api &instance;

}; // class api

} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_api_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
