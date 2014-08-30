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
#include <utility>
#include "async/delay.hh"
#include "async/Future.hh"
#include "async/Promise.hh"
#include "async/SharedFuture.hh"
#include "common/nop.hh"
#include "common/trial.hh"

namespace {

using sesh::async::Future;
using sesh::async::Promise;
using sesh::async::SharedFuture;
using sesh::async::createFailedFutureOf;
using sesh::async::createFuture;
using sesh::async::createFutureOf;
using sesh::async::createPromiseFuturePair;
using sesh::async::delay;
using sesh::common::nop;
using sesh::common::trial;

class ThrowingCopyable {
public:
    ThrowingCopyable() = default;
    ThrowingCopyable(const ThrowingCopyable &) { throw 1; }
    ThrowingCopyable(ThrowingCopyable &&) = default;
};

TEST_CASE("Shared future: default construction and invalidness") {
    SharedFuture<int> f;
    CHECK_FALSE(f.isValid());
    CHECK_FALSE(f);
}

TEST_CASE("Shared future: construction from future and invalidness") {
    SharedFuture<int> f((Future<int>()));
    CHECK_FALSE(f.isValid());
    CHECK_FALSE(f);
}

TEST_CASE("Shared future: construction from future and validness") {
    auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f((Future<int>(d)));
    d = nullptr;
    CHECK(f.isValid());
    CHECK(f);
}

TEST_CASE("Shared future: is default constructible") {
    SharedFuture<int> f;
    (void) f;
}

TEST_CASE("Shared future: is copy constructible") {
    const SharedFuture<int> f1 = Future<int>();
    const SharedFuture<int> f2(f1);
    (void) f2;
}

TEST_CASE("Shared future: is copy assignable") {
    const SharedFuture<int> f1 = Future<int>();
    SharedFuture<int> f2;
    f2 = f1;
}

TEST_CASE("Shared future: is mutually comparable") {
    const SharedFuture<int> invalid1 = Future<int>();
    const SharedFuture<int> invalid2 = Future<int>();
    CHECK(invalid1 == invalid2);
    CHECK_FALSE(invalid1 != invalid2);

    const SharedFuture<int> valid1 =
                Future<int>(std::make_shared<delay<int>>());
    const SharedFuture<int> copy1(valid1);
    const SharedFuture<int> valid2 =
                Future<int>(std::make_shared<delay<int>>());
    CHECK(valid1 == copy1);
    CHECK_FALSE(valid1 != copy1);
    CHECK_FALSE(valid1 == valid2);
    CHECK(valid1 != valid2);

    CHECK_FALSE(valid1 == invalid1);
    CHECK(valid1 != invalid1);
}

TEST_CASE("Shared future: is comparable with null pointer") {
    const SharedFuture<int> invalid = Future<int>();
    CHECK(invalid == nullptr);
    CHECK(nullptr == invalid);
    CHECK_FALSE(invalid != nullptr);
    CHECK_FALSE(nullptr != invalid);

    const SharedFuture<int> valid =
                Future<int>(std::make_shared<delay<int>>());
    CHECK_FALSE(valid == nullptr);
    CHECK_FALSE(nullptr == valid);
    CHECK(valid != nullptr);
    CHECK(nullptr != valid);
}

TEST_CASE("Shared future: validness after adding callback") {
    const auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f = Future<int>(d);
    f.then(nop());
    CHECK(f.isValid());
}

TEST_CASE("Shared future: callbacks added before setting result") {
    const auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f = Future<int>(d);

    int i = 0, j = 0;
    f.then([&i](const trial<int> &r) { i = *r; });
    f.then([f, &j](const trial<int> &) {
        f.then([&j](const trial<int> &r) { j = *r; });
    });

    CHECK(i == 0);
    CHECK(j == 0);
    d->set_result(1);
    CHECK(i == 1);
    CHECK(j == 1);
}

TEST_CASE("Shared future: callbacks added after setting result") {
    const auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f = Future<int>(d);
    d->set_result(1);

    int i = 0, j = 0;
    CHECK(i == 0);
    CHECK(j == 0);
    f.then([&i](const trial<int> &r) { i = *r; });
    f.then([f, &j](const trial<int> &) {
        f.then([&j](const trial<int> &r) { j = *r; });
    });
    CHECK(i == 1);
    CHECK(j == 1);
}

TEST_CASE("Shared future: then") {
    const auto dly = std::make_shared<delay<int>>();
    const SharedFuture<int> f1 = Future<int>(dly);
    std::pair<Promise<double>, Future<double>> pf2 =
            createPromiseFuturePair<double>();

    double d = 0.0;
    f1.then(
            [](const trial<int> &t) { return *t * 2.0; },
            std::move(pf2.first));
    std::move(pf2.second).then([&d](trial<double> &&t) { d = *t; });

    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(d == 2.0);

    int i = 0;
    f1.then([](const trial<int> &t) -> int {
        throw *t * 3;
    }).then([&i](trial<int> &&t) {
        try {
            *t;
        } catch (int v) {
            i = v;
        }
    });
    CHECK(i == 3);
}

TEST_CASE("Shared future: map") {
    const auto dly = std::make_shared<delay<int>>();
    const SharedFuture<int> f1 = Future<int>(dly);
    std::pair<Promise<double>, Future<double>> pf2 =
            createPromiseFuturePair<double>();

    double d = 0.0;
    f1.map([](const int &i) { return i * 2.0; }, std::move(pf2.first));
    std::move(pf2.second).then([&d](trial<double> &&t) { d = *t; });

    CHECK(d == 0.0);
    dly->set_result(1);
    CHECK(d == 2.0);

    int i = 0;
    f1.map([](const int &j) -> int {
        throw j * 3;
    }).then([&i](trial<int> &&t) {
        try {
            *t;
        } catch (int v) {
            i = v;
        }
    });
    CHECK(i == 3);
}

TEST_CASE("Shared future: recover, success") {
    const auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f1 = Future<int>(d);
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();

    const auto f = [](std::exception_ptr) -> int {
        FAIL("unexpected exception");
        return 2;
    };
    f1.recover(f, std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);
}

TEST_CASE("Shared future: recover, failure") {
    const auto dly = std::make_shared<delay<int>>();
    const SharedFuture<int> f1 = Future<int>(dly);

    int i = 0;
    f1.recover([](std::exception_ptr e) -> int {
        try {
            std::rethrow_exception(e);
        } catch (double d) {
            CHECK(d == 1.0);
            return 2;
        }
    }).then([&i](trial<int> &&t) {
        i = *t;
    });

    CHECK(i == 0);
    dly->set_result(std::make_exception_ptr(1.0));
    CHECK(i == 2);

    double d = 0.0;
    f1.recover([](std::exception_ptr e) -> int {
        try {
            std::rethrow_exception(e);
        } catch (double f) {
            throw 2.0 * f;
        }
    }).then([&d](trial<int> &&t) {
        try {
            *t;
        } catch (double f) {
            d = f;
        }
    });
    CHECK(d == 2.0);
}

TEST_CASE("Shared future: forward") {
    const auto d = std::make_shared<delay<int>>();
    const SharedFuture<int> f1 = Future<int>(d);
    std::pair<Promise<int>, Future<int>> pf2 = createPromiseFuturePair<int>();
    std::pair<Promise<int>, Future<int>> pf3 = createPromiseFuturePair<int>();

    int i = 0;
    f1.forward(std::move(pf2.first));
    std::move(pf2.second).then([&i](trial<int> &&r) { i = *r; });

    CHECK(i == 0);
    d->set_result(1);
    CHECK(i == 1);

    f1.forward(std::move(pf3.first));
    std::move(pf3.second).then([&i](trial<int> &&r) { i = *r + 1; });
    CHECK(i == 2);
}

TEST_CASE("Shared future: wrap, success") {
    const SharedFuture<int> f1 = createFutureOf(123);
    std::pair<Promise<Future<int>>, Future<Future<int>>> pf2 =
            createPromiseFuturePair<Future<int>>();
    f1.wrap(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<Future<int>> &&r) {
        std::move(*r).then([&i](const trial<int> &r) {
            i = *r;
        });
    });
    CHECK(i == 123);

    f1.wrap().then([&i](trial<Future<int>> &&r) {
        std::move(*r).then([&i](const trial<int> &r) {
            i = 2 * *r;
        });
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: wrap, failure in original future") {
    const SharedFuture<int> f1 = createFailedFutureOf<int>(1.0);
    std::pair<Promise<Future<int>>, Future<Future<int>>> pf2 =
            createPromiseFuturePair<Future<int>>();
    f1.wrap(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<Future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);

    f1.wrap().then([&d](trial<Future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = 2.0 * v;
        }
    });
    CHECK(d == 2.0);
}

TEST_CASE("Shared future: wrap, throwing copy constructor") {
    const SharedFuture<ThrowingCopyable> f = createFuture<ThrowingCopyable>();

    int i = 0;
    f.wrap().then([&i](trial<Future<ThrowingCopyable>> &&t) {
        REQUIRE(t.has_value());
        std::move(*t).then([&i](trial<ThrowingCopyable> &&t) {
            try {
                *t;
            } catch (int v) {
                i = v;
            }
        });
    });
    CHECK(i == 1);
}

TEST_CASE("Shared future: wrap shared, success") {
    const SharedFuture<int> f1 = createFutureOf(123);
    std::pair<Promise<SharedFuture<int>>, Future<SharedFuture<int>>> pf2 =
            createPromiseFuturePair<SharedFuture<int>>();
    f1.wrapShared(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<SharedFuture<int>> &&r) {
        r->then([&i](const trial<int> &r) {
            i = *r;
        });
    });
    CHECK(i == 123);

    f1.wrapShared().then([&i](trial<SharedFuture<int>> &&r) {
        r->then([&i](const trial<int> &r) {
            i = 2 * *r;
        });
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: wrap shared, failure in original future") {
    const SharedFuture<int> f1 = createFailedFutureOf<int>(1.0);
    std::pair<Promise<SharedFuture<int>>, Future<SharedFuture<int>>> pf2 =
            createPromiseFuturePair<SharedFuture<int>>();
    f1.wrapShared(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<SharedFuture<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);

    f1.wrapShared().then([&d](trial<SharedFuture<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = 2.0 * v;
        }
    });
    CHECK(d == 2.0);
}

TEST_CASE("Shared future: wrap shared, throwing copy constructor") {
    const SharedFuture<ThrowingCopyable> f = createFuture<ThrowingCopyable>();

    int i = 0;
    f.wrapShared().then([&i](trial<SharedFuture<ThrowingCopyable>> &&t) {
        REQUIRE(t.has_value());
        t->then([&i](const trial<ThrowingCopyable> &t) {
            try {
                *t;
            } catch (int v) {
                i = v;
            }
        });
    });
    CHECK(i == 1);
}

TEST_CASE("Shared future: unwrap, success") {
    const SharedFuture<int> f1 = createFutureOf(123);
    const SharedFuture<SharedFuture<int>> f2 = f1.wrapShared();
    Future<int> f3 = f2.unwrap();
    int i = 0;
    std::move(f3).then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);

    std::pair<Promise<int>, Future<int>> pf = createPromiseFuturePair<int>();
    f2.unwrap(std::move(pf.first));
    std::move(pf.second).then([&i](trial<int> &&t) {
        i = 2 * *t;
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: unwrap, failure in first") {
    const SharedFuture<SharedFuture<int>> f =
            createFailedFutureOf<SharedFuture<int>>(1.0);
    double d = 0.0;
    f.unwrap().then([&d](trial<int> &&t) {
        try {
            *t;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Shared future: unwrap, failure in second") {
    const SharedFuture<int> f1 = createFailedFutureOf<int>(1.0);
    const SharedFuture<SharedFuture<int>> f2 = createFutureOf(f1);
    double d = 0.0;
    f2.unwrap().then([&d](trial<int> &&t) {
        try {
            *t;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future: unwrap shared, to promise, success") {
    const SharedFuture<int> f1 = createFutureOf(123);
    Future<SharedFuture<int>> f2 = f1.wrapShared();
    std::pair<Promise<int>, Future<int>> pf3 = createPromiseFuturePair<int>();
    std::move(f2).unwrap(std::move(pf3.first));

    int i = 0;
    std::move(pf3.second).then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);
}

TEST_CASE("Future: unwrap shared, returning future, success") {
    const SharedFuture<int> f1 = createFutureOf(123);
    Future<SharedFuture<int>> f2 = f1.wrapShared();
    int i = 0;
    std::move(f2).unwrap().then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);
}

TEST_CASE("Future: unwrap shared, failure in first") {
    Future<SharedFuture<int>> f = createFailedFutureOf<SharedFuture<int>>(1.0);
    double d = 0.0;
    std::move(f).unwrap().then([&d](trial<int> &&t) {
        try {
            *t;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

TEST_CASE("Future: unwrap shared, failure in second") {
    const SharedFuture<int> f1 = createFailedFutureOf<int>(1.0);
    Future<SharedFuture<int>> f2 = createFutureOf(f1);
    double d = 0.0;
    std::move(f2).unwrap().then([&d](trial<int> &&t) {
        try {
            *t;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
