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

#ifndef INCLUDED_common_TypeTag_hh
#define INCLUDED_common_TypeTag_hh

#include "buildconfig.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace sesh {
namespace common {

namespace type_tag_impl {

template<std::size_t SIZE>
struct TypeTagValue;

template<>
struct TypeTagValue<1> {
    enum class type { V0, };
};

template<>
struct TypeTagValue<2> {
    enum class type { V0, V1, };
};

template<>
struct TypeTagValue<3> {
    enum class type { V0, V1, V2, };
};

template<>
struct TypeTagValue<4> {
    enum class type { V0, V1, V2, V3, };
};

template<>
struct TypeTagValue<5> {
    enum class type { V0, V1, V2, V3, V4, };
};

template<>
struct TypeTagValue<6> {
    enum class type { V0, V1, V2, V3, V4, V5, };
};

template<>
struct TypeTagValue<7> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, };
};

template<>
struct TypeTagValue<8> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, };
};

template<>
struct TypeTagValue<9> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, V8, };
};

template<>
struct TypeTagValue<10> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, };
};

template<>
struct TypeTagValue<11> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, };
};

template<>
struct TypeTagValue<12> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, };
};

template<>
struct TypeTagValue<13> {
    enum class type { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, };
};

template<>
struct TypeTagValue<14> {
    enum class type {
        V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, };
};

template<>
struct TypeTagValue<15> {
    enum class type {
        V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, };
};

template<>
struct TypeTagValue<16> {
    enum class type {
        V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15,
    };
};

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<std::is_same<T, Head>::value, int>::type
indexOf() noexcept {
    return 0;
}

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<!std::is_same<T, Head>::value, int>::type
indexOf() noexcept {
    return indexOf<T, Tail...>() + 1;
}

} // namespace type_tag_impl

/**
 * The type tag is an enumeration-like type to render distinct values which
 * each corresponds to one of the template parameter types.
 *
 * Note that the specialization {@code TypeTag<>} cannot be instantiated since
 * it cannot represent any type.
 */
template<typename...>
class TypeTag;

/**
 * An object of this TypeTag class template specialization always represents
 * the template parameter type T.
 */
template<typename T>
class TypeTag<T> {

public:

    /**
     * @return the number of types that may be represented by this type tag
     * specialization, which is 1 for this template specialization.
     */
    constexpr static std::size_t variety() noexcept { return 1; }

    /** The underlying enumeration type. */
    using Value = typename type_tag_impl::TypeTagValue<variety()>::type;

    /**
     * This class template specialization is default-constructible because all
     * objects represent the same type.
     */
    constexpr TypeTag() noexcept = default;

    /**
     * Constructs a type tag object from the specified underlying enumerator.
     */
    constexpr explicit TypeTag(Value) noexcept { }

    /**
     * Converts this tag to its underlying enumerator. This conversion is
     * mainly for use in the switch statement.
     */
    constexpr /*explicit*/ operator Value() const noexcept {
        return Value::V0;
    }

    /**
     * Applies the argument function to this type tag.
     * @tparam F A type which is callable with TypeTag.
     * @param f A reference to the function object that is called.
     * @return The result of the function call.
     */
    template<typename F>
    constexpr auto apply(F &&f) const
            noexcept(noexcept(std::forward<F>(f)(std::declval<TypeTag>())))
            -> decltype(std::forward<F>(f)(std::declval<TypeTag>())) {
        return std::forward<F>(f)(TypeTag());
    }

}; // template<typename T> class TypeTag<T>

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator==(TypeTag<T>, TypeTag<T>) noexcept {
    return true;
}

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator<(TypeTag<T>, TypeTag<T>) noexcept {
    return false;
}

/**
 * An object of this TypeTag class template specialization represents one of
 * the template parameter types.
 */
template<typename T1, typename T2, typename... TN>
class TypeTag<T1, T2, TN...> {

public:

    /**
     * @return the number of types that may be represented by this type tag
     * specialization.
     */
    constexpr static std::size_t variety() noexcept {
        return sizeof...(TN) + 2;
    }

    /** The underlying enumeration type. */
    using Value = typename type_tag_impl::TypeTagValue<variety()>::type;

private:

    Value mValue;

public:

    /**
     * Constructs a type tag object from the specified underlying enumerator.
     * @param v A valid underlying enumerator. If the value is not any of the
     * valid enumerators of the enumeration type, the behavior is undefined.
     */
    constexpr explicit TypeTag(Value v) noexcept : mValue(v) { }

    /**
     * Constructs a type tag object that represents the type represented by the
     * argument type tag.
     * @tparam T The type represented by the new type tag object.
     */
    template<typename T>
    constexpr TypeTag(TypeTag<T>) noexcept :
            mValue(static_cast<Value>(
                        type_tag_impl::indexOf<T, T1, T2, TN...>())) { }

private:

    class Converter {
    public:
        template<typename T>
        constexpr TypeTag operator()(TypeTag<T> t) const noexcept { return t; }
    }; // class Converter

public:

    /**
     * Constructs a type tag object that represents the type represented by the
     * argument type tag. The argument's representable types {@code U1, U2,
     * UN...} must be a subset of the new object's representable types {@code
     * T1, T2, TN...}.
     */
    template<typename U1, typename U2, typename... UN>
    constexpr TypeTag(TypeTag<U1, U2, UN...> t) noexcept :
            TypeTag(t.apply(Converter())) { }

    /** @return true if and only if the two objects represent the same type. */
    constexpr bool operator==(TypeTag that) const noexcept {
        return this->mValue == that.mValue;
    }

    /** Compares two type tags. */
    constexpr bool operator<(TypeTag that) const noexcept {
        return this->mValue < that.mValue;
    }

    /**
     * Converts this tag to its underlying enumerator. This conversion is
     * mainly for use in the switch statement.
     */
    constexpr /*explicit*/ operator Value() const noexcept {
        return mValue;
    }

    /**
     * Applies the argument function to this type tag.
     * @tparam F A type which is callable with {@code TypeTag<T>} for any type
     * T that may be represented by this type tag. The return type must be the
     * same for all possible {@code TypeTag<T>} argument.
     * @param f A reference to the function object that is called.
     * @return The result of the function call.
     */
    template<typename F>
    constexpr auto apply(F &&f) const
            noexcept(
                noexcept(std::declval<F>()(std::declval<TypeTag<T1>>())) &&
                noexcept(std::declval<TypeTag<T2, TN...>>().apply(
                    std::declval<F>())))
            -> decltype(std::declval<F>()(std::declval<TypeTag<T1>>())) {
        static_assert(
                std::is_same<
                    decltype(std::declval<F>()(std::declval<TypeTag<T1>>())),
                    decltype(std::declval<F>()(std::declval<TypeTag<T2>>()))
                >::value,
                "The return types must be the same");
        using TypeTag2 = TypeTag<T2, TN...>;
        using Value2 = typename TypeTag2::Value;
        return mValue == Value::V0 /* *this == TypeTag<T1>() */ ?
                std::forward<F>(f)(TypeTag<T1>()) :
                TypeTag2(static_cast<Value2>(static_cast<int>(mValue) - 1))
                    .apply(std::forward<F>(f));
    }

}; // class TypeTag<T1, T2, TN...>

template<typename T, typename T1, typename T2, typename... TN>
constexpr bool operator==(TypeTag<T> l, TypeTag<T1, T2, TN...> r) noexcept {
    return r == l; // "l" is implicitly converted to "decltype(r)".
}

template<typename T, typename T1, typename T2, typename... TN>
constexpr bool operator<(TypeTag<T> l, TypeTag<T1, T2, TN...> r) noexcept {
    return decltype(r)(l) < r;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_TypeTag_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
