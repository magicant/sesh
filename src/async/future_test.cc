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

#include <exception>
#include <tuple>
#include <utility>
#include "async/delay.hh"
#include "async/future.hh"
#include "async/future_test_helper.hh"
#include "async/promise.hh"
#include "catch.hpp"
#include "common/either.hh"
#include "common/nop.hh"

namespace {

using sesh::async::delay;
using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future;
using sesh::async::make_future_from;
using sesh::async::make_future_of;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::common::nop;
using sesh::common::trial;

struct move_only {
    move_only() = default;
    move_only(move_only &&) = default;
};

TEST_CASE("Future, default construction and invalidness") {
    future<future<int>> f;
    CHECK_FALSE(f.is_valid());
}

TEST_CASE("Future, construction and validness") {
    auto d = std::make_shared<delay<int>>();
    future<int> f(d);
    d = nullptr;
    CHECK(f.is_valid());
}

TEST_CASE("Future, invalidness after setting callback") {
    const auto d = std::make_shared<delay<int>>();
    future<int> f(d);
    std::move(f).then(nop());
    CHECK_FALSE(f.is_valid());
}

TEST_CASE("Future, setting callback") {
    const auto d = std::make_shared<delay<int>>();
    future<int> f(d);

    int i = 0;
    std::move(f).then([&i](trial<int> &&r) { i = r.get(); });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, invalidness in callback") {
    auto d = std::make_shared<delay<int>>();
    d->set_result(0);
    future<int> f(d);
    std::move(f).then([&f](trial<int> &&) { CHECK_FALSE(f.is_valid()); });
}

TEST_CASE("Create promise/future pair") {
    std::pair<promise<int>, future<int>> &&pf =
            make_promise_future_pair<int>();
    std::move(pf.first).set_result(123);

    int i = 0;
    std::move(pf.second).then([&i](trial<int> &&r) { i = r.get(); });
    CHECK(i == 123);
}

TEST_CASE("Future, then, to promise, success") {
    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);
    std::pair<promise<double>, future<double>> pf2 =
            make_promise_future_pair<double>();

    int i = 0;
    const auto f = [&i](trial<int> &&v) -> double { i = v.get(); return 2.0; };
    std::move(f1).then(f, std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<double> &&r) { d = r.get(); });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, then, returning future, success") {
    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);

    int i = 0;
    const auto f = [&i](trial<int> &&v) -> double { i = v.get(); return 2.0; };
    future<double> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = r.get(); });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, then, returning future, failure") {
    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);

    const auto f = [](trial<int> &&) -> char { throw 2.0; };
    future<char> f2 = std::move(f1).then(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            r.get();
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
    future<int> f1(dly);
    std::pair<promise<double>, future<double>> pf2 =
            make_promise_future_pair<double>();

    int i = 0;
    const auto f = [&i](int &&j) -> double { i = j; return 2.0; };
    std::move(f1).map(f, std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<double> &&r) { d = r.get(); });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, returning future, success, movable function") {
    class movable_function {
    private:
        int &m_i;
    public:
        explicit movable_function(int &i) noexcept : m_i(i) { }
        movable_function(movable_function &&) = default;
        double operator()(int &&v) noexcept {
            m_i = v;
            return 2.0;
        }
    };

    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);

    int i = 0;
    future<double> f2 = std::move(f1).map(movable_function(i));

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = r.get(); });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE(
        "Future, map, returning future, success, copyable constant function") {
    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);

    int i = 0;
    const auto f = [&i](int &&v) -> double { i = v; return 2.0; };
    future<double> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<double> &&r) { d = r.get(); });

    CHECK(i == 0);
    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(i == 1);
    CHECK(d == 2.0);
}

TEST_CASE("Future, map, returning future, failure propagation") {
    const auto dly = std::make_shared<delay<int>>();
    future<int> f1(dly);

    const auto f = [](int &&) -> char { FAIL("unexpected"); return 'a'; };
    future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            r.get();
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
    future<int> f1(dly);

    int i = 0;
    const auto f = [&i](int &&v) -> char { i = v; throw 2.0; };
    future<char> f2 = std::move(f1).map(f);

    double d = 0.0;
    std::move(f2).then([&d](trial<char> &&r) {
        try {
            r.get();
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
    future<int> f1(d);
    std::pair<promise<int>, future<int>> pf2 = make_promise_future_pair<int>();

    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    std::move(f1).recover(f, std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<int> &&r) { i = r.get(); });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover, returning future, success, movable function") {
    class movable_function {
    public:
        movable_function() = default;
        movable_function(movable_function &&) = default;
        int operator()(std::exception_ptr) {
            FAIL("unexpected exception");
            return 2;
        }
    };

    const auto d = std::make_shared<delay<int>>();
    future<int> f1(d);
    future<int> f2 = std::move(f1).recover(movable_function());

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) { i = r.get(); });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE(
        "Future, recover, returning future, success, "
        "copyable constant function") {
    const auto d = std::make_shared<delay<int>>();
    future<int> f1(d);
    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) { i = r.get(); });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Future, recover from exception") {
    const auto d = std::make_shared<delay<int>>();
    future<int> f1(d);
    const auto f = [](std::exception_ptr e) -> int {
        try {
            std::rethrow_exception(e);
        } catch (double d) {
            CHECK(d == 1.0);
            return 1;
        }
    };
    future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) { i = r.get(); });

    CHECK(i == 0);
    d->set_result(std::make_exception_ptr(1.0));
    CHECK(i == 1);
}

TEST_CASE("Future, recovery failure") {
    const auto d = std::make_shared<delay<int>>();
    future<int> f1(d);
    const auto f = [](std::exception_ptr) -> int { throw 2.0; };
    future<int> f2 = std::move(f1).recover(f);

    int i = 0;
    std::move(f2).then([&i](trial<int> &&r) {
        try {
            r.get();
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
    future<int> f = make_future_from([] { return 42; });
    std::move(f).then([&i](trial<int> &&r) { i = r.get(); });
    CHECK(i == 42);
}

TEST_CASE("Future, create by result construction, l-value") {
    using T = std::tuple<int, char, double>;
    future<T> f = make_future<T>(1, 'a', 2.0);
    int i = 0;
    char c = '0';
    double d = 0.0;
    std::move(f).then([&i, &c, &d](trial<T> &&r) {
        std::tie(i, c, d) = r.get();
    });
    CHECK(i == 1);
    CHECK(c == 'a');
    CHECK(d == 2.0);
}

TEST_CASE("Future, create by result construction, r-value") {
    make_future<move_only>(move_only());
}

TEST_CASE("Future, create from existing value") {
    expect_result(make_future_of(move_only()), [](move_only &&) { });
}

TEST_CASE("Future, create from exception") {
    expect_trial(make_failed_future_of<int>(1.0), [](trial<int> &&r) {
        try {
            r.get();
        } catch (double d) {
            CHECK(d == 1.0);
        }
    });
}

TEST_CASE("Future, forward, success, int") {
    std::pair<promise<int>, future<int>> pf1 = make_promise_future_pair<int>();
    std::pair<promise<int>, future<int>> pf2 = make_promise_future_pair<int>();
    std::move(pf1.first).set_result(123);
    std::move(pf1.second).forward(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<int> &&r) { i = r.get(); });
    CHECK(i == 123);
}

TEST_CASE("Future, forward, success, move only object") {
    auto pf1 = make_promise_future_pair<move_only>();
    auto pf2 = make_promise_future_pair<move_only>();
    std::move(pf1.first).set_result(move_only());
    std::move(pf1.second).forward(std::move(pf2.first));
    expect_result(std::move(pf2.second), [](trial<move_only> &&) { });
}

TEST_CASE("Future, forward, failure") {
    std::pair<promise<int>, future<int>> pf1 = make_promise_future_pair<int>();
    std::pair<promise<int>, future<int>> pf2 = make_promise_future_pair<int>();
    std::move(pf1.first).set_result_from([]() -> int { throw 1.0; });
    std::move(pf1.second).forward(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<int> &&r) {
        try {
            r.get();
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, wrap, to promise, success") {
    std::pair<promise<future<int>>, future<future<int>>> pf =
            make_promise_future_pair<future<int>>();
    make_future_of(123).wrap(std::move(pf.first));
    int i = 0;
    std::move(pf.second).then([&i](trial<future<int>> &&r) {
        std::move(r.get()).then([&i](trial<int> &&r) {
            i = r.get();
        });
    });
    CHECK(i == 123);
}

TEST_CASE("Future, wrap, returning value, success") {
    int i = 0;
    make_future_of(123).wrap().then([&i](trial<future<int>> &&r) {
        std::move(r.get()).then([&i](trial<int> &&r) {
            i = r.get();
        });
    });
    CHECK(i == 123);
}

TEST_CASE("Future, wrap, returning value, failure in original future") {
    future<future<int>> f = make_failed_future_of<int>(1.0).wrap();
    double d = 0.0;
    std::move(f).then([&d](trial<future<int>> &&r) {
        try {
            r.get();
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, unwrap, to promise, success") {
    std::pair<promise<int>, future<int>> pf = make_promise_future_pair<int>();
    make_future_of(make_future_of(123)).unwrap(std::move(pf.first));
    int i = 0;
    std::move(pf.second).then([&i](trial<int> &&r) { i = r.get(); });
    CHECK(i == 123);
}

TEST_CASE("Future, unwrap, returning future, success") {
    future<int> f = make_future_of(make_future_of(123)).unwrap();
    int i = 0;
    std::move(f).then([&i](trial<int> &&r) { i = r.get(); });
    CHECK(i == 123);
}

TEST_CASE("Future, unwrap, failure in first") {
    future<int> f = make_future_of(make_failed_future_of<int>(1.0)).unwrap();
    double d = 0.0;
    std::move(f).then([&d](trial<int> &&r) {
        try {
            r.get();
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future, unwrap, failure in second") {
    double d = 0.0;
    make_future_of(0).map([](int &&) -> future<int> {
        throw 1.0;
    }).unwrap().then([&d](trial<int> &&r) {
        try {
            r.get();
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
