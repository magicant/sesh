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
#include "async/delay.hh"
#include "async/Promise.hh"
#include "common/trial.hh"

namespace {

using sesh::async::delay;
using sesh::async::Promise;
using sesh::common::trial;

TEST_CASE("Promise, default construction and invalidness") {
    Promise<Promise<int>> p;
    CHECK_FALSE(p.is_valid());
}

TEST_CASE("Promise, construction and validness") {
    std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    d = nullptr;
    CHECK(p.is_valid());
}

TEST_CASE("Promise, invalidness after setting result") {
    const std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    std::move(p).setResult(0);
    CHECK_FALSE(p.is_valid());
}

TEST_CASE("Promise, setting result by construction, value") {
    using P = std::pair<int, double>;
    const std::shared_ptr<delay<P>> dly = std::make_shared<delay<P>>();
    Promise<P> p(dly);
    std::move(p).setResult(1, 2.0);

    int i = 0;
    double d = 0.0;
    dly->set_callback([&i, &d](trial<P> &&r) { std::tie(i, d) = *r; });
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Promise, setting result by construction, invalidness") {
    auto d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    d->set_callback([&p](trial<int> &&) { CHECK_FALSE(p.is_valid()); });
    std::move(p).setResult(0);
}

TEST_CASE("Promise, setting result by function, value") {
    const std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    std::move(p).setResultFrom([] { return 1; });

    int i = 0;
    d->set_callback([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 1);
}

TEST_CASE("Promise, setting result by function, invalidness") {
    auto d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    d->set_callback([&p](trial<int> &&) { CHECK_FALSE(p.is_valid()); });
    std::move(p).setResultFrom([] { return 0; });
}

TEST_CASE("Promise, setting result to exception, value") {
    const std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    std::move(p).fail(std::make_exception_ptr('\1'));

    char c = '\0';
    d->set_callback([&c](trial<int> &&r) {
        try {
            *r;
        } catch (char c2) {
            c = c2;
        }
    });
    CHECK(c == '\1');
}

TEST_CASE("Promise, setting result to exception, invalidness") {
    auto d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    d->set_callback([&p](trial<int> &&) { CHECK_FALSE(p.is_valid()); });
    std::move(p).fail(std::make_exception_ptr(0));
}

TEST_CASE("Promise, setting result to current exception, value") {
    const std::shared_ptr<delay<int>> d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    try {
        throw '\1';
    } catch (...) {
        std::move(p).failWithCurrentException();
    }

    char c = '\0';
    d->set_callback([&c](trial<int> &&r) {
        try {
            *r;
        } catch (char c2) {
            c = c2;
        }
    });
    CHECK(c == '\1');
}

TEST_CASE("Promise, setting result to current exception, invalidness") {
    auto d = std::make_shared<delay<int>>();
    Promise<int> p(d);
    d->set_callback([&p](trial<int> &&) { CHECK_FALSE(p.is_valid()); });
    try {
        throw 0;
    } catch (...) {
        std::move(p).failWithCurrentException();
    }
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
