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

#include <utility>
#include "async/DelayHolder.hh"

namespace {

using sesh::async::delay;
using sesh::async::DelayHolder;

TEST_CASE("Delay holder, move") {
    DelayHolder<int> d;
    d = DelayHolder<int>(std::move(d));
}

TEST_CASE("Delay holder, default construction and invalidness") {
    DelayHolder<int> dh;
    CHECK_FALSE(dh.isValid());
}

TEST_CASE("Delay holder, construction with delay and validness") {
    std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    DelayHolder<int> dh(d);
    d.reset();
    CHECK(dh.isValid());
}

TEST_CASE("Delay holder, invalidation") {
    const std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    DelayHolder<int> dh(d);
    dh.invalidate();
    CHECK_FALSE(dh.isValid());
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
