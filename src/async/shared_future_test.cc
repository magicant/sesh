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
#include <utility>
#include "async/delay.hh"
#include "async/future.hh"
#include "async/promise.hh"
#include "async/shared_future.hh"
#include "catch.hpp"
#include "common/nop.hh"
#include "common/trial.hh"

namespace {

using sesh::async::delay;
using sesh::async::future;
using sesh::async::make_failed_future_of;
using sesh::async::make_future;
using sesh::async::make_future_of;
using sesh::async::make_promise_future_pair;
using sesh::async::promise;
using sesh::async::shared_future;
using sesh::common::nop;
using sesh::common::trial;

class throwing_copyable {
public:
    throwing_copyable() = default;
    throwing_copyable(const throwing_copyable &) { throw 1; }
    throwing_copyable(throwing_copyable &&) = default;
};

TEST_CASE("Shared future: default construction and invalidness") {
    shared_future<int> f;
    CHECK_FALSE(f.is_valid());
    CHECK_FALSE(f);
}

TEST_CASE("Shared future: construction from future and invalidness") {
    shared_future<int> f((future<int>()));
    CHECK_FALSE(f.is_valid());
    CHECK_FALSE(f);
}

TEST_CASE("Shared future: construction from future and validness") {
    auto d = std::make_shared<delay<int>>();
    const shared_future<int> f((future<int>(d)));
    d = nullptr;
    CHECK(f.is_valid());
    CHECK(f);
}

TEST_CASE("Shared future: is default constructible") {
    shared_future<int> f;
    (void) f;
}

TEST_CASE("Shared future: is copy constructible") {
    const shared_future<int> f1 = future<int>();
    const shared_future<int> f2(f1);
    (void) f2;
}

TEST_CASE("Shared future: is copy assignable") {
    const shared_future<int> f1 = future<int>();
    shared_future<int> f2;
    f2 = f1;
}

TEST_CASE("Shared future: is mutually comparable") {
    const shared_future<int> invalid1 = future<int>();
    const shared_future<int> invalid2 = future<int>();
    CHECK(invalid1 == invalid2);
    CHECK_FALSE(invalid1 != invalid2);

    const shared_future<int> valid1 =
                future<int>(std::make_shared<delay<int>>());
    const shared_future<int> copy1(valid1);
    const shared_future<int> valid2 =
                future<int>(std::make_shared<delay<int>>());
    CHECK(valid1 == copy1);
    CHECK_FALSE(valid1 != copy1);
    CHECK_FALSE(valid1 == valid2);
    CHECK(valid1 != valid2);

    CHECK_FALSE(valid1 == invalid1);
    CHECK(valid1 != invalid1);
}

TEST_CASE("Shared future: is comparable with null pointer") {
    const shared_future<int> invalid = future<int>();
    CHECK(invalid == nullptr);
    CHECK(nullptr == invalid);
    CHECK_FALSE(invalid != nullptr);
    CHECK_FALSE(nullptr != invalid);

    const shared_future<int> valid =
                future<int>(std::make_shared<delay<int>>());
    CHECK_FALSE(valid == nullptr);
    CHECK_FALSE(nullptr == valid);
    CHECK(valid != nullptr);
    CHECK(nullptr != valid);
}

TEST_CASE("Shared future: validness after adding callback") {
    const auto d = std::make_shared<delay<int>>();
    const shared_future<int> f = future<int>(d);
    f.then(nop());
    CHECK(f.is_valid());
}

TEST_CASE("Shared future: callbacks added before setting result") {
    const auto d = std::make_shared<delay<int>>();
    const shared_future<int> f = future<int>(d);

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
    const shared_future<int> f = future<int>(d);
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
    const shared_future<int> f1 = future<int>(dly);
    std::pair<promise<double>, future<double>> pf2 =
            make_promise_future_pair<double>();

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
    const shared_future<int> f1 = future<int>(dly);
    std::pair<promise<double>, future<double>> pf2 =
            make_promise_future_pair<double>();

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
    const shared_future<int> f1 = future<int>(d);
    std::pair<promise<int>, future<int>> pf2 = make_promise_future_pair<int>();

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
    const shared_future<int> f1 = future<int>(dly);

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
    const shared_future<int> f1 = future<int>(d);
    std::pair<promise<int>, future<int>> pf2 = make_promise_future_pair<int>();
    std::pair<promise<int>, future<int>> pf3 = make_promise_future_pair<int>();

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
    const shared_future<int> f1 = make_future_of(123);
    std::pair<promise<future<int>>, future<future<int>>> pf2 =
            make_promise_future_pair<future<int>>();
    f1.wrap(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<future<int>> &&r) {
        std::move(*r).then([&i](const trial<int> &r) {
            i = *r;
        });
    });
    CHECK(i == 123);

    f1.wrap().then([&i](trial<future<int>> &&r) {
        std::move(*r).then([&i](const trial<int> &r) {
            i = 2 * *r;
        });
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: wrap, failure in original future") {
    const shared_future<int> f1 = make_failed_future_of<int>(1.0);
    std::pair<promise<future<int>>, future<future<int>>> pf2 =
            make_promise_future_pair<future<int>>();
    f1.wrap(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);

    f1.wrap().then([&d](trial<future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = 2.0 * v;
        }
    });
    CHECK(d == 2.0);
}

TEST_CASE("Shared future: wrap, throwing copy constructor") {
    const shared_future<throwing_copyable> f =
            make_future<throwing_copyable>();

    int i = 0;
    f.wrap().then([&i](trial<future<throwing_copyable>> &&t) {
        REQUIRE(t);
        std::move(*t).then([&i](trial<throwing_copyable> &&t) {
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
    const shared_future<int> f1 = make_future_of(123);
    std::pair<promise<shared_future<int>>, future<shared_future<int>>> pf2 =
            make_promise_future_pair<shared_future<int>>();
    f1.wrap_shared(std::move(pf2.first));

    int i = 0;
    std::move(pf2.second).then([&i](trial<shared_future<int>> &&r) {
        r->then([&i](const trial<int> &r) {
            i = *r;
        });
    });
    CHECK(i == 123);

    f1.wrap_shared().then([&i](trial<shared_future<int>> &&r) {
        r->then([&i](const trial<int> &r) {
            i = 2 * *r;
        });
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: wrap shared, failure in original future") {
    const shared_future<int> f1 = make_failed_future_of<int>(1.0);
    std::pair<promise<shared_future<int>>, future<shared_future<int>>> pf2 =
            make_promise_future_pair<shared_future<int>>();
    f1.wrap_shared(std::move(pf2.first));

    double d = 0.0;
    std::move(pf2.second).then([&d](trial<shared_future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = v;
        }
    });
    CHECK(d == 1.0);

    f1.wrap_shared().then([&d](trial<shared_future<int>> &&r) {
        try {
            *r;
        } catch (double v) {
            d = 2.0 * v;
        }
    });
    CHECK(d == 2.0);
}

TEST_CASE("Shared future: wrap shared, throwing copy constructor") {
    const shared_future<throwing_copyable> f =
            make_future<throwing_copyable>();

    int i = 0;
    f.wrap_shared().then([&i](trial<shared_future<throwing_copyable>> &&t) {
        REQUIRE(t);
        t->then([&i](const trial<throwing_copyable> &t) {
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
    const shared_future<int> f1 = make_future_of(123);
    const shared_future<shared_future<int>> f2 = f1.wrap_shared();
    future<int> f3 = f2.unwrap();
    int i = 0;
    std::move(f3).then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);

    std::pair<promise<int>, future<int>> pf = make_promise_future_pair<int>();
    f2.unwrap(std::move(pf.first));
    std::move(pf.second).then([&i](trial<int> &&t) {
        i = 2 * *t;
    });
    CHECK(i == 246);
}

TEST_CASE("Shared future: unwrap, failure in first") {
    const shared_future<shared_future<int>> f =
            make_failed_future_of<shared_future<int>>(1.0);
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
    const shared_future<int> f1 = make_failed_future_of<int>(1.0);
    const shared_future<shared_future<int>> f2 = make_future_of(f1);
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
    const shared_future<int> f1 = make_future_of(123);
    future<shared_future<int>> f2 = f1.wrap_shared();
    std::pair<promise<int>, future<int>> pf3 = make_promise_future_pair<int>();
    std::move(f2).unwrap(std::move(pf3.first));

    int i = 0;
    std::move(pf3.second).then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);
}

TEST_CASE("Future: unwrap shared, returning future, success") {
    const shared_future<int> f1 = make_future_of(123);
    future<shared_future<int>> f2 = f1.wrap_shared();
    int i = 0;
    std::move(f2).unwrap().then([&i](trial<int> &&t) {
        i = *t;
    });
    CHECK(i == 123);
}

TEST_CASE("Future: unwrap shared, failure in first") {
    future<shared_future<int>> f =
            make_failed_future_of<shared_future<int>>(1.0);
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
    const shared_future<int> f1 = make_failed_future_of<int>(1.0);
    future<shared_future<int>> f2 = make_future_of(f1);
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
