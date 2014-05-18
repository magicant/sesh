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

#include <exception>
#include <tuple>
#include <utility>
#include "async/Delay.hh"
#include "async/Future.hh"
#include "async/Promise.hh"
#include "common/Nop.hh"
#include "common/Try.hh"

namespace {

using sesh::async::Delay;
using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createFailedFutureOf;
using sesh::async::createFuture;
using sesh::async::createFutureFrom;
using sesh::async::createFutureOf;
using sesh::async::createPromiseFuturePair;
using sesh::common::Nop;
using sesh::common::Try;

struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly &&) = default;
};

TEST_CASE("Future, default construction and invalidness") {
    Future<Future<int>> f;
    CHECK_FALSE(f.isValid());
}

TEST_CASE("Future, construction and validness") {
    auto delay = std::make_shared<Delay<int>>();
    Future<int> f(delay);
    delay = nullptr;
    CHECK(f.isValid());
}

TEST_CASE("Future, invalidness after setting callback") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f(delay);
    std::move(f).setCallback(Nop());
    CHECK_FALSE(f.isValid());
}

TEST_CASE("Future, setting callback") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f(delay);

    int i = 0;
    std::move(f).setCallback([&i](Try<int> &&r) { i = *r; });

    CHECK(i == 0);
    delay->setResult(1);
    CHECK(i == 1);
}

TEST_CASE("Create promise/future pair") {
    std::pair<Promise<int>, Future<int>> &&pf = createPromiseFuturePair<int>();
    std::move(pf.first).setResult(123);

    int i = 0;
    std::move(pf.second).setCallback([&i](Try<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, then, success") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    int i = 0;
    const auto f = [&i](Try<int> &&v) -> double { i = *v; return 2.0; };
    Future<double> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    delay->setResult(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, then, failure") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    const auto f = [](Try<int> &&) -> char { throw 2.0; };
    Future<char> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(d == 0.0);
    delay->setResult(1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, success, movable function") {
    class MovableFunction {
    private:
        int &mI;
    public:
        explicit MovableFunction(int &i) noexcept : mI(i) { }
        MovableFunction(MovableFunction &&) = default;
        double operator()(int &&v) noexcept {
            mI = v;
            return 2.0;
        }
    };

    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    int i = 0;
    Future<double> f2 = std::move(f1).map(MovableFunction(i));

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    delay->setResult(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, success, copyable constant function") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    int i = 0;
    const auto f = [&i](int &&v) -> double { i = v; return 2.0; };
    Future<double> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    delay->setResult(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, failure propagation") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    const auto f = [](int &&) -> char { FAIL("unexpected"); return 'a'; };
    Future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(d == 0.0);
    delay->setResultFrom([]() -> int { throw 2.0; });
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, failure in callback") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);

    int i = 0;
    const auto f = [&i](int &&v) -> char { i = v; throw 2.0; };
    Future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).setCallback([&d](Try<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(i == 0);
    CHECK(d == 0.0);
    delay->setResult(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, recover, success, movable function") {
    class MovableFunction {
    public:
        MovableFunction() = default;
        MovableFunction(MovableFunction &&) = default;
        int operator()(std::exception_ptr) {
            FAIL("unexpected exception");
            return 2;
        }
    };

    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);
    Future<int> f2 = std::move(f1).recover(MovableFunction());

    int i = 0;
    std::move(f2).setCallback([&i](Try<int> &&r) { i = *r; });

    CHECK(i == 0);
    delay->setResult(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover, success, copyable constant function") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);
    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    Future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).setCallback([&i](Try<int> &&r) { i = *r; });

    CHECK(i == 0);
    delay->setResult(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover from exception") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);
    const auto f = [](std::exception_ptr e) -> int {
        try {
            std::rethrow_exception(e);
        } catch (double d) {
            CHECK(d == 1.0);
            return 1;
        }
    };
    Future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).setCallback([&i](Try<int> &&r) { i = *r; });

    CHECK(i == 0);
    delay->setResultFrom([]() -> int { throw 1.0; });
    CHECK(i == 1);
}

TEST_CASE("Future, recovery failure") {
    const auto delay = std::make_shared<Delay<int>>();
    Future<int> f1(delay);
    const auto f = [](std::exception_ptr) -> int { throw 2.0; };
    Future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).setCallback([&i](Try<int> &&r) {
        try {
            *r;
        } catch (double d) {
            CHECK(d == 2.0);
            i = 1;
        }
    });

    CHECK(i == 0);
    delay->setResultFrom([]() -> int { throw 1.0; });
    CHECK(i == 1);
}

TEST_CASE("Future, create from function") {
    int i = 0;
    Future<int> f = createFutureFrom([] { return 42; });
    std::move(f).setCallback([&i](Try<int> &&r) { i = *r; });
    CHECK(i == 42);
}

TEST_CASE("Future, create by result construction, l-value") {
    using T = std::tuple<int, char, double>;
    Future<T> f = createFuture<T>(1, 'a', 2.0);
    int i = 0;
    char c = '0';
    double d = 0.0;
    std::move(f).setCallback([&i, &c, &d](Try<T> &&r) {
        std::tie(i, c, d) = *r;
    });
    CHECK(i == 1);
    CHECK(c == 'a');
    CHECK(d == 2.0);
}

TEST_CASE("Future, create by result construction, r-value") {
    createFuture<MoveOnly>(MoveOnly());
}

TEST_CASE("Future, create from existing value") {
    bool called = false;
    createFutureOf(MoveOnly()).setCallback([&called](Try<MoveOnly> &&r) {
        CHECK_NOTHROW(*r);
        called = true;
    });
    CHECK(called);
}

TEST_CASE("Future, create from exception") {
    bool called = false;
    createFailedFutureOf<int>(1.0).setCallback([&called](Try<int> &&r) {
        try {
            *r;
        } catch (double d) {
            CHECK(d == 1.0);
            called = true;
        }
    });
    CHECK(called);
}

TEST_CASE("Future, forward, success, int") {
    std::pair<Promise<int>, Future<int>> pf1 = createPromiseFuturePair<int>();
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();
    std::move(pf1.first).setResult(123);
    std::move(pf1.second).forward(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).setCallback([&i](Try<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, forward, success, move only object") {
    auto pf1 = createPromiseFuturePair<MoveOnly>();
    auto pf2 = createPromiseFuturePair<MoveOnly>();
    std::move(pf1.first).setResult(MoveOnly());
    std::move(pf1.second).forward(std::move(pf2.first));

    bool called = false;
    std::move(pf2.second).setCallback(
            [&called](Try<MoveOnly> &&) { called = true; });
    CHECK(called);
}

TEST_CASE("Future, forward, failure") {
    std::pair<Promise<int>, Future<int>> pf1 = createPromiseFuturePair<int>();
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();
    std::move(pf1.first).setResultFrom([]() -> int { throw 1.0; });
    std::move(pf1.second).forward(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).setCallback([&d](Try<int> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, unwrap, success") {
    Future<int> f = createFutureOf(createFutureOf(123)).unwrap();
    int i = 0;
    std::move(f).setCallback([&i](Try<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, unwrap, failure in first") {
    Future<int> f = createFutureOf(createFailedFutureOf<int>(1.0)).unwrap();
    double d = 0.0;
    std::move(f).setCallback([&d](Try<int> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, unwrap, failure in second") {
    double d = 0.0;
    createFutureOf(0).map([](int &&) -> Future<int> {
        throw 1.0;
    }).unwrap().setCallback([&d](Try<int> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
