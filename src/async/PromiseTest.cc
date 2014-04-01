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
#include "async/Delay.hh"
#include "async/Promise.hh"
#include "common/Try.hh"

namespace {

using sesh::async::Delay;
using sesh::async::Promise;
using sesh::common::Try;

TEST_CASE("Promise, default construction and invalidness") {
    Promise<Promise<int>> p;
    CHECK_FALSE(p.isValid());
}

TEST_CASE("Promise, construction and validness") {
    std::shared_ptr<Delay<int>> delay = std::make_shared<Delay<int>>();
    Promise<int> p(delay);
    delay = nullptr;
    CHECK(p.isValid());
}

TEST_CASE("Promise, invalidness after setting result") {
    const std::shared_ptr<Delay<int>> delay = std::make_shared<Delay<int>>();
    Promise<int> p(delay);
    std::move(p).setResultFrom([] { return 0; });
    CHECK_FALSE(p.isValid());
}

TEST_CASE("Promise, setting result") {
    const std::shared_ptr<Delay<int>> delay = std::make_shared<Delay<int>>();
    Promise<int> p(delay);
    std::move(p).setResultFrom([] { return 1; });

    int i = 0;
    delay->setCallback([&i](Try<int> &&r) { i = *r; });
    CHECK(i == 1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
