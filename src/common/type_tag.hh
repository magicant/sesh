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

#ifndef INCLUDED_common_type_tag_hh
#define INCLUDED_common_type_tag_hh

#include "buildconfig.h"

#include <cstddef>
#include <type_traits>
#include <utility>
#include "common/function_helper.hh"
#include "common/logic_helper.hh"
#include "helpermacros.h"

namespace sesh {
namespace common {

namespace type_tag_impl {

template<std::size_t SIZE>
struct type_tag_value;

template<>
struct type_tag_value<0> {
    enum class type { };
};

template<>
struct type_tag_value<1> {
    enum class type { v0, };
};

template<>
struct type_tag_value<2> {
    enum class type { v0, v1, };
};

template<>
struct type_tag_value<3> {
    enum class type { v0, v1, v2, };
};

template<>
struct type_tag_value<4> {
    enum class type { v0, v1, v2, v3, };
};

template<>
struct type_tag_value<5> {
    enum class type { v0, v1, v2, v3, v4, };
};

template<>
struct type_tag_value<6> {
    enum class type { v0, v1, v2, v3, v4, v5, };
};

template<>
struct type_tag_value<7> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, };
};

template<>
struct type_tag_value<8> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, };
};

template<>
struct type_tag_value<9> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, v8, };
};

template<>
struct type_tag_value<10> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, };
};

template<>
struct type_tag_value<11> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, };
};

template<>
struct type_tag_value<12> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, };
};

template<>
struct type_tag_value<13> {
    enum class type { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, };
};

template<>
struct type_tag_value<14> {
    enum class type {
        v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, };
};

template<>
struct type_tag_value<15> {
    enum class type {
        v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, };
};

template<>
struct type_tag_value<16> {
    enum class type {
        v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15,
    };
};

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<std::is_same<T, Head>::value, int>::type
index_of() noexcept {
    return 0;
}

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<!std::is_same<T, Head>::value, int>::type
index_of() noexcept {
    return index_of<T, Tail...>() + 1;
}

} // namespace type_tag_impl

/**
 * The type tag is an enumeration-like type to render distinct values which
 * each corresponds to one of the template parameter types.
 *
 * Note that the specialization <code>type_tag<></code> cannot be instantiated
 * since it cannot represent any type.
 */
template<typename...>
class type_tag;

/** The empty type tag. This class cannot be instantiated. */
template<>
class type_tag<> {

public:

    /**
     * @return the number of types that may be represented by this type tag
     * specialization, which is 0 for this template specialization.
     */
    constexpr static std::size_t variety() noexcept { return 0; }

    /** The underlying enumeration type. */
    using value_type = typename type_tag_impl::type_tag_value<0>::type;

    constexpr type_tag() noexcept = delete;
    constexpr explicit type_tag(value_type) noexcept = delete;

    constexpr /*explicit*/ operator value_type() const noexcept = delete;

    template<typename F>
    [[noreturn]] constexpr void apply(F &&f) const = delete;

}; // template<> class type_tag<>

/**
 * An object of this type_tag class template specialization always represents
 * the template parameter type T.
 */
template<typename T>
class type_tag<T> {

public:

    /**
     * @return the number of types that may be represented by this type tag
     * specialization, which is 1 for this template specialization.
     */
    constexpr static std::size_t variety() noexcept { return 1; }

    /** The underlying enumeration type. */
    using value_type = typename type_tag_impl::type_tag_value<variety()>::type;

    /**
     * This class template specialization is default-constructible because all
     * objects represent the same type.
     */
    constexpr type_tag() noexcept = default;

    /**
     * Constructs a type tag object from the specified underlying enumerator.
     */
    constexpr explicit type_tag(value_type) noexcept { }

    /**
     * Converts this tag to its underlying enumerator. This conversion is
     * mainly for use in the switch statement.
     */
    constexpr /*explicit*/ operator value_type() const noexcept {
        return value_type::v0;
    }

    /**
     * Applies the argument function to this type tag.
     * @tparam F A type which is callable with type_tag.
     * @param f A reference to the function object that is called.
     * @return The result of the function call.
     */
    template<typename F>
    constexpr auto apply(F &&f) const
            noexcept(is_nothrow_callable<F(type_tag)>::value)
            -> typename result_of<F(type_tag)>::type {
        return invoke(std::forward<F>(f), type_tag());
    }

}; // template<typename T> class type_tag<T>

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator==(type_tag<T>, type_tag<T>) noexcept {
    return true;
}

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator<(type_tag<T>, type_tag<T>) noexcept {
    return false;
}

/**
 * An object of this type_tag class template specialization represents one of
 * the template parameter types.
 */
template<typename T1, typename T2, typename... TN>
class type_tag<T1, T2, TN...> {

public:

    /**
     * @return the number of types that may be represented by this type tag
     * specialization.
     */
    constexpr static std::size_t variety() noexcept {
        return sizeof...(TN) + 2;
    }

    /** The underlying enumeration type. */
    using value_type = typename type_tag_impl::type_tag_value<variety()>::type;

private:

    value_type m_value;

public:

    /**
     * Constructs a type tag object from the specified underlying enumerator.
     * @param v A valid underlying enumerator. If the value is not any of the
     * valid enumerators of the enumeration type, the behavior is undefined.
     */
    constexpr explicit type_tag(value_type v) noexcept : m_value(v) { }

    /**
     * Constructs a type tag object that represents the type represented by the
     * argument type tag.
     * @tparam T The type represented by the new type tag object.
     */
    template<typename T>
    constexpr type_tag(type_tag<T>) noexcept :
            m_value(static_cast<value_type>(
                        type_tag_impl::index_of<T, T1, T2, TN...>())) { }

private:

    class converter {
    public:
        template<typename T>
        constexpr type_tag operator()(type_tag<T> t) const noexcept {
            return t;
        }
    }; // class converter

public:

    /**
     * Constructs a type tag object that represents the type represented by the
     * argument type tag. The argument's representable types <code>U1, U2,
     * UN...</code> must be a subset of the new object's representable types
     * <code>T1, T2, TN...</code>.
     */
    template<typename U1, typename U2, typename... UN>
    constexpr type_tag(type_tag<U1, U2, UN...> t) noexcept :
            type_tag(t.apply(converter())) { }

    /** @return true if and only if the two objects represent the same type. */
    constexpr bool operator==(type_tag that) const noexcept {
        return this->m_value == that.m_value;
    }

    /** Compares two type tags. */
    constexpr bool operator<(type_tag that) const noexcept {
        return this->m_value < that.m_value;
    }

    /**
     * Converts this tag to its underlying enumerator. This conversion is
     * mainly for use in the switch statement.
     */
    constexpr /*explicit*/ operator value_type() const noexcept {
        return m_value;
    }

    /**
     * Applies the argument function to this type tag.
     * @tparam F A type which is callable with <code>type_tag&lt;T></code> for
     * any type @c T that may be represented by this type tag. The return type
     * must be the same for all possible <code>type_tag&lt;T></code> argument.
     * @param f A reference to the function object that is called.
     * @return The result of the function call.
     */
    template<typename F>
    constexpr auto apply(F &&f) const
            noexcept(for_all<
                    is_nothrow_callable<F(type_tag<T1>)>::value,
                    is_nothrow_callable<F(type_tag<T2>)>::value,
                    is_nothrow_callable<F(type_tag<TN>)>::value...>::value)
            -> typename common_result<
                    F, type_tag<T1>, type_tag<T2>, type_tag<TN>...>::type;

}; // class type_tag<T1, T2, TN...>

namespace type_tag_impl {

template<typename T0, typename T1, typename F>
auto apply(F &&f, type_tag_value<2>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value)
        -> typename common_result<F, type_tag<T0>, type_tag<T1>>::type {
    switch (v) {
    case type_tag_value<2>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<2>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    }
    UNREACHABLE();
}

template<typename T0, typename T1, typename T2, typename F>
auto apply(F &&f, type_tag_value<3>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>>::type {
    switch (v) {
    case type_tag_value<3>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<3>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<3>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    }
    UNREACHABLE();
}

template<typename T0, typename T1, typename T2, typename T3, typename F>
auto apply(F &&f, type_tag_value<4>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>
        >::type {
    switch (v) {
    case type_tag_value<4>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<4>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<4>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<4>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename F>
auto apply(F &&f, type_tag_value<5>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>>::type {
    switch (v) {
    case type_tag_value<5>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<5>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<5>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<5>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<5>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename F>
auto apply(F &&f, type_tag_value<6>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>>::type {
    switch (v) {
    case type_tag_value<6>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<6>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<6>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<6>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<6>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<6>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename F>
auto apply(F &&f, type_tag_value<7>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>>::type {
    switch (v) {
    case type_tag_value<7>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<7>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<7>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<7>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<7>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<7>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<7>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename F>
auto apply(F &&f, type_tag_value<8>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>>::type {
    switch (v) {
    case type_tag_value<8>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<8>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<8>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<8>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<8>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<8>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<8>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<8>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename F>
auto apply(F &&f, type_tag_value<9>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>>::type {
    switch (v) {
    case type_tag_value<9>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<9>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<9>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<9>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<9>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<9>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<9>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<9>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<9>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename F>
auto apply(F &&f, type_tag_value<10>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>>::type {
    switch (v) {
    case type_tag_value<10>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<10>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<10>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<10>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<10>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<10>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<10>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<10>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<10>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<10>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename F>
auto apply(F &&f, type_tag_value<11>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>>::type {
    switch (v) {
    case type_tag_value<11>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<11>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<11>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<11>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<11>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<11>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<11>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<11>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<11>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<11>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<11>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename T11, typename F>
auto apply(F &&f, type_tag_value<12>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value &&
                is_nothrow_callable<F(type_tag<T11>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>, type_tag<T11>
                >::type {
    switch (v) {
    case type_tag_value<12>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<12>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<12>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<12>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<12>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<12>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<12>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<12>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<12>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<12>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<12>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    case type_tag_value<12>::type::v11:
        return invoke(std::forward<F>(f), type_tag<T11>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename T11, typename T12, typename F>
auto apply(F &&f, type_tag_value<13>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value &&
                is_nothrow_callable<F(type_tag<T11>)>::value &&
                is_nothrow_callable<F(type_tag<T12>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>, type_tag<T11>,
                type_tag<T12>>::type {
    switch (v) {
    case type_tag_value<13>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<13>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<13>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<13>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<13>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<13>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<13>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<13>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<13>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<13>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<13>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    case type_tag_value<13>::type::v11:
        return invoke(std::forward<F>(f), type_tag<T11>());
    case type_tag_value<13>::type::v12:
        return invoke(std::forward<F>(f), type_tag<T12>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename T11, typename T12, typename T13, typename F>
auto apply(F &&f, type_tag_value<14>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value &&
                is_nothrow_callable<F(type_tag<T11>)>::value &&
                is_nothrow_callable<F(type_tag<T12>)>::value &&
                is_nothrow_callable<F(type_tag<T13>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>, type_tag<T11>,
                type_tag<T12>, type_tag<T13>>::type {
    switch (v) {
    case type_tag_value<14>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<14>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<14>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<14>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<14>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<14>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<14>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<14>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<14>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<14>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<14>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    case type_tag_value<14>::type::v11:
        return invoke(std::forward<F>(f), type_tag<T11>());
    case type_tag_value<14>::type::v12:
        return invoke(std::forward<F>(f), type_tag<T12>());
    case type_tag_value<14>::type::v13:
        return invoke(std::forward<F>(f), type_tag<T13>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename T11, typename T12, typename T13, typename T14,
        typename F>
auto apply(F &&f, type_tag_value<15>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value &&
                is_nothrow_callable<F(type_tag<T11>)>::value &&
                is_nothrow_callable<F(type_tag<T12>)>::value &&
                is_nothrow_callable<F(type_tag<T13>)>::value &&
                is_nothrow_callable<F(type_tag<T14>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>, type_tag<T11>,
                type_tag<T12>, type_tag<T13>, type_tag<T14>>::type {
    switch (v) {
    case type_tag_value<15>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<15>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<15>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<15>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<15>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<15>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<15>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<15>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<15>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<15>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<15>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    case type_tag_value<15>::type::v11:
        return invoke(std::forward<F>(f), type_tag<T11>());
    case type_tag_value<15>::type::v12:
        return invoke(std::forward<F>(f), type_tag<T12>());
    case type_tag_value<15>::type::v13:
        return invoke(std::forward<F>(f), type_tag<T13>());
    case type_tag_value<15>::type::v14:
        return invoke(std::forward<F>(f), type_tag<T14>());
    }
    UNREACHABLE();
}

template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9,
        typename T10, typename T11, typename T12, typename T13, typename T14,
        typename T15, typename F>
auto apply(F &&f, type_tag_value<16>::type v)
        noexcept(
                is_nothrow_callable<F(type_tag<T0>)>::value &&
                is_nothrow_callable<F(type_tag<T1>)>::value &&
                is_nothrow_callable<F(type_tag<T2>)>::value &&
                is_nothrow_callable<F(type_tag<T3>)>::value &&
                is_nothrow_callable<F(type_tag<T4>)>::value &&
                is_nothrow_callable<F(type_tag<T5>)>::value &&
                is_nothrow_callable<F(type_tag<T6>)>::value &&
                is_nothrow_callable<F(type_tag<T7>)>::value &&
                is_nothrow_callable<F(type_tag<T8>)>::value &&
                is_nothrow_callable<F(type_tag<T9>)>::value &&
                is_nothrow_callable<F(type_tag<T10>)>::value &&
                is_nothrow_callable<F(type_tag<T11>)>::value &&
                is_nothrow_callable<F(type_tag<T12>)>::value &&
                is_nothrow_callable<F(type_tag<T13>)>::value &&
                is_nothrow_callable<F(type_tag<T14>)>::value &&
                is_nothrow_callable<F(type_tag<T15>)>::value)
        -> typename common_result<
                F, type_tag<T0>, type_tag<T1>, type_tag<T2>, type_tag<T3>,
                type_tag<T4>, type_tag<T5>, type_tag<T6>, type_tag<T7>,
                type_tag<T8>, type_tag<T9>, type_tag<T10>, type_tag<T11>,
                type_tag<T12>, type_tag<T13>, type_tag<T14>, type_tag<T15>
                >::type {
    switch (v) {
    case type_tag_value<16>::type::v0:
        return invoke(std::forward<F>(f), type_tag<T0>());
    case type_tag_value<16>::type::v1:
        return invoke(std::forward<F>(f), type_tag<T1>());
    case type_tag_value<16>::type::v2:
        return invoke(std::forward<F>(f), type_tag<T2>());
    case type_tag_value<16>::type::v3:
        return invoke(std::forward<F>(f), type_tag<T3>());
    case type_tag_value<16>::type::v4:
        return invoke(std::forward<F>(f), type_tag<T4>());
    case type_tag_value<16>::type::v5:
        return invoke(std::forward<F>(f), type_tag<T5>());
    case type_tag_value<16>::type::v6:
        return invoke(std::forward<F>(f), type_tag<T6>());
    case type_tag_value<16>::type::v7:
        return invoke(std::forward<F>(f), type_tag<T7>());
    case type_tag_value<16>::type::v8:
        return invoke(std::forward<F>(f), type_tag<T8>());
    case type_tag_value<16>::type::v9:
        return invoke(std::forward<F>(f), type_tag<T9>());
    case type_tag_value<16>::type::v10:
        return invoke(std::forward<F>(f), type_tag<T10>());
    case type_tag_value<16>::type::v11:
        return invoke(std::forward<F>(f), type_tag<T11>());
    case type_tag_value<16>::type::v12:
        return invoke(std::forward<F>(f), type_tag<T12>());
    case type_tag_value<16>::type::v13:
        return invoke(std::forward<F>(f), type_tag<T13>());
    case type_tag_value<16>::type::v14:
        return invoke(std::forward<F>(f), type_tag<T14>());
    case type_tag_value<16>::type::v15:
        return invoke(std::forward<F>(f), type_tag<T15>());
    }
    UNREACHABLE();
}

} // namespace type_tag_impl

template<typename T1, typename T2, typename... TN>
template<typename F>
constexpr auto type_tag<T1, T2, TN...>::apply(F &&f) const
        noexcept(for_all<
                is_nothrow_callable<F(type_tag<T1>)>::value,
                is_nothrow_callable<F(type_tag<T2>)>::value,
                is_nothrow_callable<F(type_tag<TN>)>::value...>::value)
        -> typename common_result<
                F, type_tag<T1>, type_tag<T2>, type_tag<TN>...>::type {
    return type_tag_impl::apply<T1, T2, TN...>(std::forward<F>(f), m_value);
}

template<typename T, typename T1, typename T2, typename... TN>
constexpr bool operator==(type_tag<T> l, type_tag<T1, T2, TN...> r) noexcept {
    return r == l; // "l" is implicitly converted to "decltype(r)".
}

template<typename T, typename T1, typename T2, typename... TN>
constexpr bool operator<(type_tag<T> l, type_tag<T1, T2, TN...> r) noexcept {
    return decltype(r)(l) < r;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_type_tag_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
