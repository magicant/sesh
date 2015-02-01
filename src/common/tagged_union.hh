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

#ifndef INCLUDED_common_tagged_union_hh
#define INCLUDED_common_tagged_union_hh

#include "buildconfig.h"

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include "common/function_helper.hh"
#include "common/type_tag.hh"

namespace sesh {
namespace common {

template<typename...>
union tagged_union_value;

template<>
union tagged_union_value<> { };

template<typename Head, typename... Tail>
union tagged_union_value<Head, Tail...> {

    static_assert(
            std::is_same<Head, typename std::decay<Head>::type>::value,
            "tagged union value must be decayed");

    Head head;
    tagged_union_value<Tail...> tail;

    tagged_union_value() noexcept { }
    ~tagged_union_value() noexcept { }

}; // union tagged_union_value<Head, Tail...>

template<typename T, typename... Tail>
constexpr T &get(tagged_union_value<T, Tail...> &tuv) noexcept {
    return tuv.head;
}

template<typename T, typename... Tail>
constexpr const T &get(const tagged_union_value<T, Tail...> &tuv) noexcept {
    return tuv.head;
}

template<typename T, typename... Tail>
constexpr T &&get(tagged_union_value<T, Tail...> &&tuv) noexcept {
    return std::move(tuv.head);
}

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<!std::is_same<T, Head>::value, T &>::type
get(tagged_union_value<Head, Tail...> &tuv) noexcept {
    return get<T>(tuv.tail);
}

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<!std::is_same<T, Head>::value, const T &>::type
get(const tagged_union_value<Head, Tail...> &tuv) noexcept {
    return get<T>(tuv.tail);
}

template<typename T, typename Head, typename... Tail>
constexpr
typename std::enable_if<!std::is_same<T, Head>::value, T &&>::type
get(tagged_union_value<Head, Tail...> &&tuv) noexcept {
    return get<T>(std::move(tuv.tail));
}

/** Type tag visitor which forwards to a variant visitor. */
template<typename Union, typename Visitor>
class union_tag_visitor {

private:

    Union &m_target;
    Visitor &m_visitor;

public:

    constexpr union_tag_visitor(Union &&target, Visitor &&visitor) noexcept :
            m_target(target), m_visitor(visitor) { }

    template<typename T>
    constexpr auto operator()(type_tag<T>) const
            noexcept(is_nothrow_callable<Visitor(
                    decltype(get<T>(std::forward<Union>(m_target))))>::value)
            -> typename std::result_of<Visitor(
                    decltype(get<T>(std::forward<Union>(m_target))))>::type {
        return invoke(
                std::forward<Visitor>(m_visitor),
                get<T>(std::forward<Union>(m_target)));
    }

}; // template<typename Union> class applier

/**
 * A tagged union is a union with a tag that indicates which element of the
 * union is active. Unlike normal unions, a tagged union cannot change its
 * active element type once constructed. However, the destructor of the tagged
 * union calls the destructor of the active element to properly finalize it.
 *
 * Tagged unions are seldom used directly. Normally you would prefer {@link
 * variant}s that can change active element types and are assignable.
 *
 * @tparam T Types of elements that may be contained in the union. They all
 * must be no-throw destructible and decayed.
 */
template<typename... T>
class tagged_union : private type_tag<T...> {

    /*
     * The type tag sub-object is contained as a base class object rather than
     * a non-static data member to allow empty base optimization.
     */

public:

    using tag_type = type_tag<T...>;

private:

    using union_value_type = tagged_union_value<T...>;

    class value_copier {

    private:

        union_value_type &m_value;

    public:

        constexpr explicit value_copier(union_value_type &v) noexcept :
                m_value(v) { }

        template<typename V, typename U = typename std::decay<V>::type>
        void operator()(V &&v) const
                noexcept(std::is_nothrow_constructible<U, V>::value) {
            new (std::addressof(get<U>(m_value))) U(std::forward<V>(v));
        }

    }; // class tagged_union_value_copier

    class move_if_noexcept_constructor {

    private:

        union_value_type &m_value;

    public:

        constexpr explicit move_if_noexcept_constructor(union_value_type &v)
                noexcept : m_value(v) { }

        template<typename V>
        void operator()(V &v) const
                noexcept(std::is_nothrow_move_constructible<V>::value) {
            new (std::addressof(get<V>(m_value))) V(std::move_if_noexcept(v));
        }

    }; // class move_if_noexcept_constructor

    union_value_type m_value;

public:

    /** Returns the tag that indicates the active element type. */
    constexpr tag_type tag() const noexcept { return *this; }

    /**
     * Returns a reference to the element of the specified type. The returned
     * reference can be dereferenced only when the specified type is the type
     * of the actually active element.
     * @tparam U Type of the element to get a reference of. It must be one of
     * the {@code T}s.
     *
     * @see get function
     */
    template<typename U>
    U &value() noexcept {
        assert(tag() == type_tag<U>());
        return get<U>(m_value);
    }

    /**
     * Returns a reference to the element of the specified type. The returned
     * reference can be dereferenced only when the specified type is the type
     * of the actually active element.
     * @tparam U Type of the element to get a reference of. It must be one of
     * the {@code T}s.
     *
     * @see get function
     */
    template<typename U>
    const U &value() const noexcept {
        assert(tag() == type_tag<U>());
        return get<U>(m_value);
    }

    /**
     * Calls the argument's () operator passing the currently contained
     * element.
     *
     * For each element type of this tagged union, the argument visitor must be
     * callable with a l-value reference to the element. The return type must
     * be the same for all the element types.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) &
            noexcept(noexcept(std::declval<tag_type &>().apply(
                std::declval<
                    union_tag_visitor<union_value_type &, Visitor>>())))
            -> typename common_result<Visitor, T &...>::type {
        return tag_type::apply(
                union_tag_visitor<union_value_type &, Visitor>(
                    m_value, std::forward<Visitor>(visitor)));
    }

    /**
     * Calls the argument's () operator passing the currently contained
     * element.
     *
     * For each element type of this tagged union, the argument visitor must be
     * callable with a l-value const reference to the element. The return type
     * must be the same for all the element types.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) const &
            noexcept(noexcept(std::declval<const tag_type &>().apply(
                std::declval<union_tag_visitor<
                    const union_value_type &, Visitor>>())))
            -> typename common_result<Visitor, const T &...>::type {
        return tag_type::apply(
                union_tag_visitor<const union_value_type &, Visitor>(
                    m_value, std::forward<Visitor>(visitor)));
    }

    /**
     * Calls the argument's () operator passing the currently contained
     * element.
     *
     * For each element type of this tagged union, the argument visitor must be
     * callable with a r-value reference to the element. The return type must
     * be the same for all the element types.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) &&
            noexcept(noexcept(std::declval<tag_type &>().apply(
                std::declval<
                    union_tag_visitor<union_value_type &&, Visitor>>())))
            -> typename common_result<Visitor, T &&...>::type {
        return tag_type::apply(
                union_tag_visitor<union_value_type &&, Visitor>(
                    std::move(m_value), std::forward<Visitor>(visitor)));
    }

    /**
     * Constructs a tagged union by direct-initializing the element of the
     * specified type with the specified arguments.
     *
     * This constructor is @c noexcept if and only if the element constructor
     * is @c noexcept.
     *
     * @tparam U Type of the element to construct.
     * @tparam A Types of the arguments that are forwarded to the constructor
     * of the element.
     * @param t Dummy argument to help deduction of template parameter @c U.
     * @param a Arguments that are forwarded to the constructor of the
     * element.
     */
    template<typename U, typename... A>
    /* constexpr */ explicit tagged_union(type_tag<U> t, A &&... a)
            noexcept(std::is_nothrow_constructible<U, A...>::value) :
            tag_type(t), m_value() {
        new (std::addressof(get<U>(m_value))) U(std::forward<A>(a)...);
    }

    /**
     * Copy constructor.
     *
     * Instantiation of this constructor requires all possible elements be
     * copy-constructible.
     */
    tagged_union(const tagged_union &tu)
            noexcept(for_all<
                    std::is_nothrow_copy_constructible<T>::value...
                    >::value) :
            tag_type(tu), m_value() {
        tu.apply(value_copier(m_value));
    }

    /**
     * Widening copy constructor.
     *
     * Instantiation of this constructor requires all possible elements of the
     * argument tagged union be copy-constructible.
     *
     * @tparam U Element types of the operand tagged union. Each of {@code U}s
     * must be one of {@code T}s.
     */
    template<typename... U>
    tagged_union(const tagged_union<U...> &tu)
            noexcept(for_all<
                    std::is_nothrow_copy_constructible<U>::value...
                    >::value) :
            tag_type(tu.tag()), m_value() {
        tu.apply(value_copier(m_value));
    }

    /**
     * Move constructor.
     *
     * Instantiation of this constructor requires all possible elements be
     * move-constructible.
     */
    tagged_union(tagged_union &&tu)
            noexcept(for_all<
                    std::is_nothrow_move_constructible<T>::value...
                    >::value) :
            tag_type(tu), m_value() {
        std::move(tu).apply(value_copier(m_value));
    }

    /**
     * Widening move constructor.
     *
     * Instantiation of this constructor requires all possible elements of the
     * argument tagged union be copy-constructible.
     *
     * @tparam U Element types of the operand tagged union. Each of {@code U}s
     * must be one of {@code T}s.
     */
    template<typename... U>
    tagged_union(tagged_union<U...> &&tu)
            noexcept(for_all<
                    std::is_nothrow_move_constructible<U>::value...
                    >::value) :
            tag_type(tu.tag()), m_value() {
        std::move(tu).apply(value_copier(m_value));
    }

    /**
     * Move-if-noexcept constructor. If the active element of the argument
     * tagged union has a never-throwing move constructor or no copy
     * constructor, the element is move-constructed. Otherwise, it is
     * copy-constructed.
     *
     * Instantiation of this constructor requires all possible elements of the
     * argument tagged union be copy-constructible.
     *
     * @tparam U Element types of the operand tagged union. Each of {@code U}s
     * must be one of {@code T}s.
     */
    template<typename... U>
    tagged_union(move_if_noexcept, tagged_union<U...> &tu)
            noexcept(for_all<
                    std::is_nothrow_move_constructible<U>::value...
                    >::value) :
            tag_type(tu.tag()), m_value() {
        tu.apply(move_if_noexcept_constructor(m_value));
    }

    /** Destructs the active element of the tagged union. */
    ~tagged_union() noexcept {
        apply(destructor());
    }

}; // template<typename... T> class tagged_union

/**
 * Returns a reference to the tagged union element of the specified type. This
 * function can be called only when @c T is the actual element type.
 * @tparam T Type of the element to get a reference of. It must be one of the
 * {@code U}s.
 */
template<typename T, typename... U>
constexpr T &get(tagged_union<U...> &tu) noexcept {
    return tu.template value<T>();
}

/**
 * Returns a reference to the tagged union element of the specified type. This
 * function can be called only when @c T is the actual element type.
 * @tparam T Type of the element to get a reference of. It must be one of the
 * {@code U}s.
 */
template<typename T, typename... U>
constexpr const T &get(const tagged_union<U...> &tu) noexcept {
    return tu.template value<T>();
}

/**
 * Returns a reference to the tagged union element of the specified type. This
 * function can be called only when @c T is the actual element type.
 * @tparam T Type of the element to get a reference of. It must be one of the
 * {@code U}s.
 */
template<typename T, typename... U>
constexpr T &&get(tagged_union<U...> &&tu) noexcept {
    return std::move(tu.template value<T>());
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_tagged_union_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
