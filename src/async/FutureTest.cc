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
#include "async/delay.hh"
#include "async/Future.hh"
#include "async/Promise.hh"
#include "common/nop.hh"
#include "common/trial.hh"

namespace {

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::createFailedFutureOf;
using sesh::async::createFuture;
using sesh::async::createFutureFrom;
using sesh::async::createFutureOf;
using sesh::async::createPromiseFuturePair;
using sesh::async::delay;
using sesh::common::nop;
using sesh::common::trial;

struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly &&) = default;
};

TEST_CASE("Future, default construction and invalidness") {
    Future<Future<int>> f;
    CHECK_FALSE(f.isValid());
}

TEST_CASE("Future, construction and validness") {
    auto d = std::make_shared<delay<int>>();
    Future<int> f(d);
    d = nullptr;
    CHECK(f.isValid());
}

TEST_CASE("Future, invalidness after setting callback") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f(d);
    std::move(f).then(nop());
    CHECK_FALSE(f.isValid());
}

TEST_CASE("Future, setting callback") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f(d);

    int i = 0;
    std::move(f).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, invalidness in callback") {
    auto d = std::make_shared<delay<int>>();
    d->set_result(0);
    Future<int> f(d);
    std::move(f).then([&f](trial<int> &&) { CHECK_FALSE(f.isValid()); });
}

TEST_CASE("Create promise/future pair") {
    std::pair<Promise<int>, Future<int>> &&pf = createPromiseFuturePair<int>();
    std::move(pf.first).setResult(123);

    int i = 0;
    std::move(pf.second).then([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, then, to promise, success") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);
    std::pair<Promise<double>, Future<double>> pf2 =
            createPromiseFuturePair<double>();

    int i = 0;
    const auto f = [&i](trial<int> &&v) -> double { i = *v; return 2.0; };
    std::move(f1).then(f, std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, then, returning future, success") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    int i = 0;
    const auto f = [&i](trial<int> &&v) -> double { i = *v; return 2.0; };
    Future<double> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, then, returning future, failure") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    const auto f = [](trial<int> &&) -> char { throw 2.0; };
    Future<char> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, to promise, success") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);
    std::pair<Promise<double>, Future<double>> pf2 =
            createPromiseFuturePair<double>();

    int i = 0;
    const auto f = [&i](int &&j) -> double { i = j; return 2.0; };
    std::move(f1).map(f, std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, returning future, success, movable function") {
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

    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    int i = 0;
    Future<double> f2 = std::move(f1).map(MovableFunction(i));

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE(
        "Future, map, returning future, success, copyable constant function") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    int i = 0;
    const auto f = [&i](int &&v) -> double { i = v; return 2.0; };
    Future<double> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = *r; });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, returning future, failure propagation") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    const auto f = [](int &&) -> char { FAIL("unexpected"); return 'a'; };
    Future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(d == 0.0);
    dly->set_result(std::make_exception_ptr(2.0));
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, failure in callback") {
    const auto dly = std::make_shared<delay<int>>();
    Future<int> f1(dly);

    int i = 0;
    const auto f = [&i](int &&v) -> char { i = v; throw 2.0; };
    Future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, recover, to promise, success") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f1(d);
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();

    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    std::move(f1).recover(f, std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover, returning future, success, movable function") {
    class MovableFunction {
    public:
        MovableFunction() = default;
        MovableFunction(MovableFunction &&) = default;
        int operator()(std::exception_ptr) {
            FAIL("unexpected exception");
            return 2;
        }
    };

    const auto d = std::make_shared<delay<int>>();
    Future<int> f1(d);
    Future<int> f2 = std::move(f1).recover(MovableFunction());

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE(
        "Future, recover, returning future, success, "
        "copyable constant function") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f1(d);
    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    Future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover from exception") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f1(d);
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
    std::move(f2).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(std::make_exception_ptr(1.0));
    CHECK(i == 1);
}

TEST_CASE("Future, recovery failure") {
    const auto d = std::make_shared<delay<int>>();
    Future<int> f1(d);
    const auto f = [](std::exception_ptr) -> int { throw 2.0; };
    Future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) {
        try {
            *r;
        } catch (double d) {
            CHECK(d == 2.0);
            i = 1;
        }
    });

    CHECK(i == 0);
    d->set_result(std::make_exception_ptr(1.0));
    CHECK(i == 1);
}

TEST_CASE("Future, create from function") {
    int i = 0;
    Future<int> f = createFutureFrom([] { return 42; });
    std::move(f).then([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 42);
}

TEST_CASE("Future, create by result construction, l-value") {
    using T = std::tuple<int, char, double>;
    Future<T> f = createFuture<T>(1, 'a', 2.0);
    int i = 0;
    char c = '0';
    double d = 0.0;
    std::move(f).then([&i, &c, &d](trial<T> &&r) {
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
    createFutureOf(MoveOnly()).then([&called](trial<MoveOnly> &&r) {
        CHECK_NOTHROW(*r);
        called = true;
    });
    CHECK(called);
}

TEST_CASE("Future, create from exception") {
    bool called = false;
    createFailedFutureOf<int>(1.0).then([&called](trial<int> &&r) {
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
    std::move(pf2.second).then([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, forward, success, move only object") {
    auto pf1 = createPromiseFuturePair<MoveOnly>();
    auto pf2 = createPromiseFuturePair<MoveOnly>();
    std::move(pf1.first).setResult(MoveOnly());
    std::move(pf1.second).forward(std::move(pf2.first));

    bool called = false;
    std::move(pf2.second).then([&called](trial<MoveOnly> &&) {
        called = true;
    });
    CHECK(called);
}

TEST_CASE("Future, forward, failure") {
    std::pair<Promise<int>, Future<int>> pf1 = createPromiseFuturePair<int>();
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();
    std::move(pf1.first).setResultFrom([]() -> int { throw 1.0; });
    std::move(pf1.second).forward(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<int> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, wrap, to promise, success") {
    std::pair<Promise<Future<int>>, Future<Future<int>>> pf =
            createPromiseFuturePair<Future<int>>();
    createFutureOf(123).wrap(std::move(pf.first));
    int i = 0;
    std::move(pf.second).then([&i](trial<Future<int>> &&r) {
        std::move(*r).then([&i](trial<int> &&r) {
            i = *r;
        });
    });
    CHECK(i == 123);
}

TEST_CASE("Future, wrap, returning value, success") {
    int i = 0;
    createFutureOf(123).wrap().then([&i](trial<Future<int>> &&r) {
        std::move(*r).then([&i](trial<int> &&r) {
            i = *r;
        });
    });
    CHECK(i == 123);
}

TEST_CASE("Future, wrap, returning value, failure in original future") {
    Future<Future<int>> f = createFailedFutureOf<int>(1.0).wrap();
    double d = 0.0;
    std::move(f).then([&d](trial<Future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, unwrap, to promise, success") {
    std::pair<Promise<int>, Future<int>> pf = createPromiseFuturePair<int>();
    createFutureOf(createFutureOf(123)).unwrap(std::move(pf.first));
    int i = 0;
    std::move(pf.second).then([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, unwrap, returning future, success") {
    Future<int> f = createFutureOf(createFutureOf(123)).unwrap();
    int i = 0;
    std::move(f).then([&i](trial<int> &&r) { i = *r; });
    CHECK(i == 123);
}

TEST_CASE("Future, unwrap, failure in first") {
    Future<int> f = createFutureOf(createFailedFutureOf<int>(1.0)).unwrap();
    double d = 0.0;
    std::move(f).then([&d](trial<int> &&r) {
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
    }).unwrap().then([&d](trial<int> &&r) {
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
