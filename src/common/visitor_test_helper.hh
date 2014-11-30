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

#ifndef INCLUDED_common_visitor_test_helper_hh
#define INCLUDED_common_visitor_test_helper_hh

#include "buildconfig.h"

#include <type_traits>
#include <typeinfo>
#include <utility>
#include "catch.hpp"
#include "common/visitor.hh"

namespace sesh {
namespace common {

/**
 * Visitor that checks if a value of the expected type is visited and passes
 * the value to the checker function.
 *
 * @tparam V Type of the visitable value that is expected.
 * @tparam F Type of the checker function that must be callable with a
 * <code>const V &</code> argument.
 */
template<typename V, typename F>
class checking_visitor {

private:

    F m_check;

public:

    template<
            typename... A,
            typename = typename std::enable_if<
                    std::is_constructible<F, A...>::value>::type>
    constexpr explicit checking_visitor(A &&... a) :
            m_check(std::forward<A>(a)...) { }

    void operator()(const V &v) const { m_check(v); }

    template<typename U>
    void operator()(const U &) const { FAIL(typeid(U).name()); }

}; // template<typename F> class checking_visitor

/** Returns a checking visitor that uses the argument checker function. */
template<typename V, typename F>
auto make_checking_visitor(F &&f)
        -> checking_visitor<V, typename std::decay<F>::type> {
    return checking_visitor<V, typename std::decay<F>::type>(
            std::forward<F>(f));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_visitor_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
