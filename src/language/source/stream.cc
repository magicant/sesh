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
#include "stream.hh"

#include <iterator>
#include "async/future.hh"

namespace {

using sesh::async::make_future;

} // namespace

namespace sesh {
namespace language {
namespace source {

stream_value_future empty_stream_value_future() {
    return make_future<stream_value>();
}

stream stream_of(const fragment_position &fp, const stream &s) {
    if (fp == nullptr)
        return s;
    return static_cast<stream_value_future>(
            make_future<stream_value>(fp, stream_of(std::next(fp), s)));
}

} // namespace source
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
