/* Copyright (C) 2015 WATANABE Yuki
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

#ifndef INCLUDED_async_future_test_helper_hh
#define INCLUDED_async_future_test_helper_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "async/future.hh"
#include "catch.hpp"
#include "common/direct_initialize.hh"
#include "common/shared_function.hh"

namespace sesh {
namespace async {

template<typename Tester>
class expect_trial_guard {

private:

    bool m_called = false;
    Tester m_tester;

public:

    constexpr expect_trial_guard(const Tester &t) : m_tester(t) { }
    constexpr expect_trial_guard(Tester &&t) : m_tester(std::move(t)) { }

    ~expect_trial_guard() { CHECK(m_called); }

    template<typename T>
    void operator()(T &&t) {
        m_called = true;
        m_tester(std::forward<T>(t));
    }

}; // template<typename V, typename Tester> class expect_trial_guard

template<typename Tester>
class expect_result_guard {

private:

    Tester m_tester;

public:

    constexpr expect_result_guard(const Tester &t) : m_tester(t) { }
    constexpr expect_result_guard(Tester &&t) : m_tester(std::move(t)) { }

    template<typename T>
    void operator()(const common::trial<T> &t) {
        REQUIRE(t);
        m_tester(*t);
    }

    template<typename T>
    void operator()(common::trial<T> &&t) {
        REQUIRE(t);
        m_tester(std::move(*t));
    }

}; // template<typename Tester> class expect_result_guard

/**
 * Adds a callback to the argument future so that the argument tester function
 * is called when the future receives a result. The @c Tester must be callable
 * with <code>trial&lt;V> &&</code>. If the future never receives a result,
 * then the test case will fail.
 */
template<typename V, typename Tester>
void expect_trial(future<V> &&f, Tester &&t) {
    using G = common::shared_function<
            expect_trial_guard<typename std::decay<Tester>::type>>;
    std::move(f).then(G(common::direct_initialize(), std::forward<Tester>(t)));
}

/**
 * Adds a callback to the argument future so that the argument tester function
 * is called when the future receives a result. The @c Tester must be callable
 * with <code>V &&</code>. If the future receives an exception or never
 * receives a result, then the test case will fail.
 */
template<typename V, typename Tester>
void expect_result(future<V> &&f, Tester &&t) {
    using G = expect_result_guard<typename std::decay<Tester>::type>;
    return expect_trial(std::move(f), G(std::forward<Tester>(t)));
}

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_future_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
