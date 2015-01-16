/* Copyright (C) 2013 WATANABE Yuki
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

#ifndef INCLUDED_common_variant_hh
#define INCLUDED_common_variant_hh

#include "buildconfig.h"

#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include "common/direct_initialize.hh"
#include "common/function_helper.hh"
#include "common/logic_helper.hh"
#include "common/type_tag.hh"

namespace sesh {
namespace common {

namespace variant_impl {

/**
 * Defined to be (a subclass of) std::true_type or std::false_type depending on
 * the template parameter types. The Boolean will be true if and only if
 * @c T is the same type as one (or more) of <code>U</code>s.
 */
template<typename T, typename... U>
class is_any_of : public for_any<std::is_same<T, U>::value...> { };

/** Contains the value of a variant. */
template<typename...>
union value_union;

template<>
union value_union<> { };

template<typename Head, typename... Tail>
union value_union<Head, Tail...> {

    static_assert(
            std::is_same<Head, typename std::decay<Head>::type>::value,
            "Contained type must be decayed");

private:

    using tail_union = value_union<Tail...>;

    Head m_head;
    tail_union m_tail;

public:

    /** The constructor does nothing. */
    value_union() noexcept { }

    /** Union cannot be copied without knowing the current value's type. */
    value_union(const value_union &) = delete;

    /** Union cannot be moved without knowing the current value's type. */
    value_union(value_union &&) = delete;

    /**
     * The destructor does nothing. The active value in this union, if any,
     * must be explicitly destructed using {@link destructor} before this
     * destructor is called.
     */
    ~value_union() noexcept { }

    /** Union cannot be assigned without knowing the current value's type. */
    value_union &operator=(const value_union &) = delete;

    /** Union cannot be assigned without knowing the current value's type. */
    value_union &operator=(value_union &&) = delete;

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, U &>::type
    value() & noexcept {
        return m_head;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, const U &>::type
    value() const & noexcept {
        return m_head;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, U &&>::type
    value() && noexcept {
        return std::move(m_head);
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, U &>::type
    value() & noexcept {
        return m_tail.template value<U>();
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, const U &>::type
    value() const & noexcept {
        return m_tail.template value<U>();
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, U &&>::type
    value() && noexcept {
        return std::move(m_tail).template value<U>();
    }

    /**
     * Constructs the value of type <code>Head</code> using the given
     * arguments.
     */
    template<typename... Arg>
    // constexpr XXX C++11 7.1.5.4
    explicit value_union(type_tag<Head>, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<Head, Arg...>::value) :
            m_head(std::forward<Arg>(arg)...) { }

    /**
     * Constructs the value of type <code>U</code> using the given arguments.
     */
    template<typename U, typename... Arg>
    // constexpr XXX C++11 7.1.5.4
    explicit value_union(type_tag<U> tag, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) :
            m_tail(tag, std::forward<Arg>(arg)...) { }

};

template<typename T, typename U>
using copy_reference = typename std::conditional<
        std::is_lvalue_reference<T>::value, U &, U &&>::type;

/** A visitor on the type tag which forwards to a visitor on the variant. */
template<typename Union, typename Visitor>
class applier {

private:

    Union &&m_target;
    Visitor &&m_visitor;

public:

    constexpr explicit applier(Union &&target, Visitor &&visitor) noexcept :
            m_target(std::forward<Union>(target)),
            m_visitor(std::forward<Visitor>(visitor)) { }

    template<typename T, typename R = copy_reference<Union, T>>
    constexpr auto operator()(type_tag<T>) const
            noexcept(is_nothrow_callable<Visitor(R)>::value)
            -> typename std::result_of<Visitor(R)>::type {
        return invoke(std::forward<Visitor>(m_visitor),
                std::forward<Union>(m_target).template value<T>());
    }

}; // template<typename Union> class applier

/** A visitor that constructs a value of a target union. */
template<typename Union>
class constructor {

private:

    Union &m_target;

public:

    constexpr explicit constructor(Union &target) noexcept :
            m_target(target) { }

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v) const
            noexcept(std::is_nothrow_constructible<V, T &&>::value) {
        new (std::addressof(m_target)) Union(
                type_tag<V>(), std::forward<T>(v));
    }

}; // template<typename Union> class constructor

/** A visitor that constructs a value of a target union. */
template<typename Union>
class move_if_noexcept_constructor {

private:

    Union &m_target;

public:

    constexpr explicit move_if_noexcept_constructor(Union &target) noexcept :
            m_target(target) { }

    template<typename T>
    void operator()(T &v) const
            noexcept(std::is_nothrow_move_constructible<T>::value) {
        new (std::addressof(m_target)) Union(
                type_tag<T>(), std::move_if_noexcept(v));
    }

}; // template<typename Union> class move_if_noexcept_constructor

/** A visitor that assigns a value to a target union. */
template<typename Union>
class assigner {

private:

    Union &m_target;

public:

    constexpr explicit assigner(Union &target) noexcept : m_target(target) { }

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v) const
            noexcept(std::is_nothrow_assignable<V &, T &&>::value) {
        m_target.template value<V>() = std::forward<T>(v);
    }

}; // template<typename Union> class assigner

namespace {

/** A helper function that constructs an applier. */
template<typename Union, typename Visitor>
applier<Union, Visitor> make_applier(Union &&u, Visitor &&v) noexcept {
    return applier<Union, Visitor>(
            std::forward<Union>(u), std::forward<Visitor>(v));
}

/** A helper function that constructs a constructor. */
template<typename Union>
constructor<Union> make_constructor(Union &target) noexcept {
    return constructor<Union>(target);
}

template<typename Union>
move_if_noexcept_constructor<Union>
make_move_if_noexcept_constructor(Union &target) noexcept {
    return move_if_noexcept_constructor<Union>(target);
}

/** A helper function that constructs an assigner. */
template<typename Union>
assigner<Union> make_assigner(Union &target) noexcept {
    return assigner<Union>(target);
}

/**
 * Re-constructs the pointed-to object by forwarding the argument to the
 * constructor. This function assumes the constructor never throws. If the
 * constructor did throw something at runtime, std::terminate is called.
 */
template<typename T, typename... Arg>
void reconstruct_or_terminate(T *t, Arg &&... arg) noexcept {
    new (t) T(std::forward<Arg>(arg)...);
}

} // namespace

namespace swap_impl {

using std::swap;

template<typename T>
class is_swappable {

private:

    template<typename U>
    static auto f(U &) -> decltype(
            (void) swap(std::declval<U &>(), std::declval<U &>()),
            std::true_type());

    template<typename>
    static auto f(...) -> std::false_type;

public:

    using type = decltype(f<T>(std::declval<T &>()));

}; // template<typename T> class is_swappable

template<typename T, bool = is_swappable<T>::type::value>
class is_nothrow_swappable_impl : public std::false_type { };

template<typename T>
class is_nothrow_swappable_impl<T, true> :
    public std::integral_constant<
            bool,
            noexcept(swap(std::declval<T &>(), std::declval<T &>()))> {
};

template<typename T>
using is_nothrow_swappable = is_nothrow_swappable_impl<T>;

template<typename Variant>
class swapper {

private:

    Variant &m_other;

public:

    constexpr explicit swapper(Variant &other) noexcept : m_other(other) { }

    template<typename T>
    void operator()(T &v) const noexcept(is_nothrow_swappable<T>::value) {
        swap(v, m_other.template value<T>());
    }

}; // template<typename Variant> class swapper

} // namespace swap_impl

template<typename F, typename O>
class mapper {

private:

    F &&m_function;

public:

    constexpr explicit mapper(F &&f) noexcept :
            m_function(std::forward<F>(f)) { }

    template<typename T>
    constexpr
    typename std::enable_if<is_callable<F(T &&)>::value, O>::type
    operator()(T &&t) const {
        return invoke(std::forward<F>(m_function), std::forward<T>(t));
    }

    template<typename T>
    constexpr
    typename std::enable_if<
            !is_callable<F(T &&)>::value &&
                    std::is_convertible<T &&, O>::value,
            O>::type
    operator()(T &&t) const {
        return std::forward<T>(t);
    }

}; // template<typename F, typename O> class mapper

template<typename F>
class map_result {

public:

    template<typename T>
    auto operator()(T &&) const
            -> typename result_of<F(T &&)>::type;

    template<typename T>
    auto operator()(T &&) const
            -> typename std::enable_if<!is_callable<F(T &&)>::value, T>::type;

}; // template<typename F> class map_result

/** Fundamental implementation of variant. */
template<typename... T>
class variant_base : private type_tag<T...> {

    /*
     * The type tag sub-object is contained as a base class object rather than
     * a non-static data member to allow empty base optimization.
     */

public:

    /**
     * The type of the type tag which specifies the type of the contained
     * value.
     */
    using tag_type = type_tag<T...>;

private:

    using value_type = value_union<T...>;

    value_type m_value;

protected:

    value_type &value() noexcept { return m_value; }
    const value_type &value() const noexcept { return m_value; }

public:

    /** The nth type of the contained types. */
    template<std::size_t n>
    using nth_type = typename std::tuple_element<n, std::tuple<T...>>::type;

    constexpr static bool is_nothrow_copy_constructible =
            for_all<std::is_nothrow_copy_constructible<T>::value...>::value;
    constexpr static bool is_nothrow_move_constructible =
            for_all<std::is_nothrow_move_constructible<T>::value...>::value;
    constexpr static bool is_nothrow_copy_assignable =
            for_all<std::is_nothrow_copy_assignable<T>::value...>::value &&
            is_nothrow_copy_constructible;
    constexpr static bool is_nothrow_move_assignable =
            for_all<std::is_nothrow_move_assignable<T>::value...>::value &&
            is_nothrow_move_constructible;
    constexpr static bool is_nothrow_swappable =
            for_all<swap_impl::is_nothrow_swappable<T>::value...>::value &&
            is_nothrow_move_constructible;

    /** Returns the integral value that identifies the parameter type. */
    template<typename U>
    constexpr static tag_type tag() noexcept {
        return type_tag<U>();
    }

    /**
     * Returns an integral value that identifies the type of the currently
     * contained value.
     */
    tag_type tag() const noexcept { return *this; }

    /**
     * Creates a new variant by constructing its contained value by calling the
     * constructor of <code>U</code> with forwarded arguments.
     *
     * Throws any exception thrown by the constructor.
     *
     * @tparam U the type of the new contained value to be constructed.
     *     (inferred from the type of type tag argument.)
     * @tparam Arg the type of the constructor's arguments.
     * @param tag a dummy object to select the type of the contained value.
     * @param arg the arguments forwarded to the constructor.
     */
    template<typename U, typename... Arg>
    explicit variant_base(direct_initialize, type_tag<U> tag, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) :
            tag_type(tag), m_value(tag, std::forward<Arg>(arg)...) { }

    /**
     * Creates a new variant by copy- or move-constructing its contained value
     * from the argument.
     *
     * Throws any exception thrown by the constructor.
     *
     * @tparam U the argument type.
     * @tparam V the type of the new contained value to be constructed.
     *     Inferred from the argument type.
     * @param v the argument forwarded to the constructor.
     */
    template<
            typename U,
            typename V = typename std::decay<U>::type,
            typename = typename std::enable_if<
                    std::is_constructible<V, U>::value>::type,
            typename = typename std::enable_if<
                    is_any_of<V, T...>::value>::type>
    variant_base(U &&v)
            noexcept(std::is_nothrow_constructible<V, U &&>::value) :
            variant_base(
                    direct_initialize(), type_tag<V>(), std::forward<U>(v)) { }

    /**
     * Returns a reference to the value of the template parameter type.
     *
     * This function can be called only when the currently contained value is
     * of the parameter type. The referenced object's lifetime ends when the
     * type of the contained value is changed or the variant is destructed.
     */
    template<typename U>
    U &value() & noexcept {
        assert(tag() == tag<U>());
        return m_value.template value<U>();
    }

    /**
     * Returns a reference to the value of the template parameter type.
     *
     * This function can be called only when the currently contained value is
     * of the parameter type. The referenced object's lifetime ends when the
     * type of the contained value is changed or the variant is destructed.
     */
    template<typename U>
    const U &value() const & noexcept {
        assert(tag() == tag<U>());
        return m_value.template value<U>();
    }

    /**
     * Returns a reference to the value of the template parameter type.
     *
     * This function can be called only when the currently contained value is
     * of the parameter type. The referenced object's lifetime ends when the
     * type of the contained value is changed or the variant is destructed.
     */
    template<typename U>
    U &&value() && noexcept {
        assert(tag() == tag<U>());
        return std::move(m_value.template value<U>());
    }

    /**
     * Calls the argument's () operator passing the currently contained value
     * of this variant.
     *
     * For each contained type of this variant, the argument visitor type must
     * have a () operator function that takes an argument of the contained
     * type. All the () operators must return values of the same type. The ()
     * operator may be a function template.
     *
     * This overload is used when the variant object is in l-value context. The
     * contained value is passed to the () operator in l-value context.
     *
     * @tparam Visitor the type of the argument.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) &
            noexcept(noexcept(std::declval<tag_type &>().apply(
                    std::declval<applier<value_type &, Visitor>>())))
            -> typename common_result<Visitor, T &...>::type {
        return tag_type::apply(make_applier(
                    value(), std::forward<Visitor>(visitor)));
    }

    /**
     * Calls the argument's () operator passing the currently contained value
     * of this variant.
     *
     * For each contained type of this variant, the argument visitor type must
     * have a () operator function that takes an argument of the contained
     * type. All the () operators must return values of the same type. The ()
     * operator may be a function template.
     *
     * This overload is used when the variant object is in const l-value
     * context. The contained value is passed to the () operator in const
     * l-value context.
     *
     * @tparam Visitor the type of the argument.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) const &
            noexcept(noexcept(std::declval<const tag_type &>().apply(
                    std::declval<applier<const value_type &, Visitor>>())))
            -> typename common_result<Visitor, const T &...>::type {
        return tag_type::apply(make_applier(
                    value(), std::forward<Visitor>(visitor)));
    }

    /**
     * Calls the argument's () operator passing the currently contained value
     * of this variant.
     *
     * For each contained type of this variant, the argument visitor type must
     * have a () operator function that takes an argument of the contained
     * type. All the () operators must return values of the same type. The ()
     * operator may be a function template.
     *
     * This overload is used when the variant object is in r-value context. The
     * contained value is passed to the () operator in r-value context.
     *
     * @tparam Visitor the type of the argument.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) &&
            noexcept(noexcept(std::declval<tag_type &>().apply(
                    std::declval<applier<value_type, Visitor>>())))
            -> typename common_result<Visitor, T &&...>::type {
        return tag_type::apply(make_applier(
                    std::move(value()), std::forward<Visitor>(visitor)));
    }

    /**
     * Destructor.
     *
     * Calls the currently contained value's destructor.
     */
    ~variant_base() noexcept {
        apply(destructor());
    }

    /**
     * Copy constructor.
     *
     * The copy constructor of the currently contained type in the argument is
     * used to initialize the value of the new variant.
     *
     * Propagates any exception thrown by the constructor.
     *
     * Requirements: All the contained types must be copy-constructible.
     */
    variant_base(const variant_base &v)
            noexcept(is_nothrow_copy_constructible) :
            tag_type(v.tag()) {
        v.apply(make_constructor(value()));
    }

    /**
     * Widening copy constructor.
     *
     * The copy constructor of the currently contained type in the argument is
     * used to initialize the value of the new variant.
     *
     * Propagates any exception thrown by the constructor.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * copy-constructible.
     */
    template<typename... U>
    variant_base(const variant_base<U...> &v)
            noexcept(variant_base<U...>::is_nothrow_copy_constructible) :
            tag_type(v.tag()) {
        v.apply(make_constructor(value()));
    }

    /**
     * Move constructor.
     *
     * The move (or copy) constructor of the currently contained type in the
     * argument is used to initialize the value of the new variant.
     *
     * Propagates any exception thrown by the constructor.
     *
     * Requirements: All the contained types must be move-constructible.
     */
    variant_base(variant_base &&v) noexcept(is_nothrow_move_constructible) :
            tag_type(v.tag()) {
        std::move(v).apply(make_constructor(value()));
    }

    /**
     * Widening move constructor.
     *
     * The move (or copy) constructor of the currently contained type in the
     * argument is used to initialize the value of the new variant.
     *
     * Propagates any exception thrown by the constructor.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * move-constructible.
     */
    template<typename... U>
    variant_base(variant_base<U...> &&v)
            noexcept(variant_base<U...>::is_nothrow_move_constructible) :
            tag_type(v.tag()) {
        std::move(v).apply(make_constructor(this->value()));
    }

    /**
     * Widening move-if-noexcept constructor.
     *
     * The move (or copy) constructor of the currently contained type in the
     * argument is used to initialize the value of the new variant. If the
     * value has a never-throwing move constructor or no copy constructor, the
     * value is move-constructed. Otherwise, it is copy-constructed.
     *
     * Propagates any exception thrown by the constructor.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * move-constructible.
     */
    template<typename... U>
    variant_base(move_if_noexcept, variant_base<U...> &v)
            noexcept(variant_base<U...>::is_nothrow_move_constructible) :
            tag_type(v.tag()) {
        v.apply(make_move_if_noexcept_constructor(value()));
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * To ensure that a valid object is contained after return from this
     * function, nothing must be thrown by the constructor of the new value. If
     * something is thrown at runtime, std::terminate is called.
     *
     * @see #emplace_with_fallback
     * @see #emplace_with_backup
     * @see #reset
     */
    template<typename... Arg>
    void emplace(Arg &&... arg) noexcept {
        this->~variant_base();
        new (this) variant_base(std::forward<Arg>(arg)...);
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * If the constructor threw something, the default constructor of @c
     * Fallback is called as a fallback and the exception is re-thrown. If the
     * @c Fallback default constructor threw again, std::terminate is called.
     *
     * Requirements: @c Fallback must be default-constructible.
     */
    template<typename Fallback, typename... Arg>
    void emplace_with_fallback(Arg &&... arg)
            noexcept(
                std::is_nothrow_constructible<variant_base, Arg &&...>::value)
    {
        try {
            this->~variant_base();
            new (this) variant_base(std::forward<Arg>(arg)...);
        } catch (...) {
            reconstruct_or_terminate(
                    this, direct_initialize(), type_tag<Fallback>());
            throw;
        }
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * This overload is selected if the constructor is guaranteed never to
     * throw. This overload is equivalent to {@link #emplace}.
     */
    template<typename... Arg>
    typename std::enable_if<
            std::is_nothrow_constructible<variant_base, Arg &&...>::value
    >::type
    emplace_with_backup(Arg &&... arg) noexcept {
        return emplace(std::forward<Arg>(arg)...);
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * This overload is selected if the constructor may throw. In case it
     * should throw something, a temporary backup of the original value is
     * created before the construction. If an exception is thrown, the backup
     * is used to restore the original value before propagating the exception.
     * If the restoration again fails with an exception, std::terminate is
     * called. The backup is created by std::move_if_noexcept and restored by
     * move-construction.
     *
     * Requirements: All the contained types must be move-constructible.
     */
    template<typename... Arg>
    typename std::enable_if<
            !std::is_nothrow_constructible<variant_base, Arg &&...>::value
    >::type
    emplace_with_backup(Arg &&... arg) {
        variant_base backup(move_if_noexcept(), *this);
        try {
            this->~variant_base();
            new (this) variant_base(std::forward<Arg>(arg)...);
        } catch (...) {
            reconstruct_or_terminate(this, std::move(backup));
            throw;
        }
    }

    /**
     * Sets the value of this variant to the argument by emplacement.
     *
     * If emplacement fails due to an exception, std::terminate is called.
     *
     * Requirements: The argument type <code>V</code> must be constructible.
     *
     * @tparam U the (usually inferred) type of the new contained value.
     * @tparam V the actual type of the new contained value.
     * @param v a reference to the original value
     *
     * @see #assign
     * @see #emplace
     */
    template<typename U, typename V = typename std::decay<U>::type>
    void reset(U &&v) noexcept {
        emplace(direct_initialize(), type_tag<V>(), std::forward<U>(v));
    }

    /**
     * Sets the value of this variant to the argument by assignment or
     * emplacement. The assignment operator is used if the currently contained
     * type is <code>V</code>. Otherwise, the argument is emplaced.
     *
     * Propagates any exception thrown by the assignment operator. If
     * emplacement fails due to an exception, std::terminate is called.
     *
     * Requirements: The argument type <code>V</code> must be constructible and
     * assignable from the argument.
     *
     * @tparam U the (usually inferred) type of the new contained value.
     * @tparam V the actual type of the new contained value.
     * @param v a reference to the original value
     *
     * @see #reset
     */
    template<typename U, typename V = typename std::decay<U>::type>
    void assign(U &&v) noexcept(std::is_nothrow_assignable<V &, U &&>::value) {
        if (tag() == tag<V>())
            value<V>() = std::forward<U>(v);
        else
            reset(std::forward<U>(v));
    }

    /**
     * Assignment from the same type is disabled by default. Implementation
     * subclasses may have their own assignment operators.
     */
    variant_base &operator=(const variant_base &) = delete;

    /**
     * Assignment from the same type is disabled by default. Implementation
     * subclasses may have their own assignment operators.
     */
    variant_base &operator=(variant_base &&) = delete;

}; // template<typename... T> class variant_base

/**
 * A subclass of conditionally copy-constructible variant that re-defines the
 * move assignment operator.
 */
template<typename... T>
class move_assignable_variant : public variant_base<T...> {

private:

    using base = variant_base<T...>;

public:

    using base::base;

    move_assignable_variant(const move_assignable_variant &) = default;
    move_assignable_variant(move_assignable_variant &&) = default;
    move_assignable_variant &operator=(const move_assignable_variant &) =
            delete;

    /**
     * Move assignment operator.
     *
     * If the left-hand-side and right-hand-side contain values of the same
     * type, the value is assigned by its move (or copy) assignment operator.
     * Otherwise, the old value is destructed and the new value is
     * move- (or copy-) constructed.
     *
     * If the move constructor for the new value may throw, a temporary copy of
     * the old value is move- or copy-constructed before the destruction so
     * that, if the constructor for the new value throws, we can restore the
     * variant to the original value.
     *
     * Propagates any exception thrown by the assignment operator or move/copy
     * constructor.
     *
     * Requirements: All the contained types must be move-constructible and
     * move-assignable.
     */
    move_assignable_variant &operator=(move_assignable_variant &&v)
            noexcept(base::is_nothrow_move_assignable) {
        if (this->tag() == v.tag())
            std::move(v).apply(make_assigner(this->value()));
        else
            this->emplace_with_backup(std::move(v));
        return *this;
    }

}; // template<typename... T> class move_assignable_variant

/**
 * Either move-assignable or conditionally copy-constructible variant class,
 * selected by move-constructibility and -assignability.
 */
template<typename... T>
using conditionally_move_assignable_variant =
        typename std::conditional<
                for_all<std::is_move_constructible<T>::value...>::value &&
                for_all<std::is_move_assignable<T>::value...>::value,
                move_assignable_variant<T...>,
                variant_base<T...>
        >::type;

/**
 * A subclass of move-assignable variant that re-defines the copy assignment
 * operator.
 */
template<typename... T>
class copy_assignable_variant : public move_assignable_variant<T...> {

private:

    using base = move_assignable_variant<T...>;

public:

    using base::base;

    copy_assignable_variant(const copy_assignable_variant &) = default;
    copy_assignable_variant(copy_assignable_variant &&) = default;
    copy_assignable_variant &operator=(copy_assignable_variant &&) = default;

    /**
     * Copy assignment operator.
     *
     * If the left-hand-side and right-hand-side contain values of the same
     * type, the value is assigned by its copy assignment operator. Otherwise,
     * the old value is destructed and the new value is copy-constructed.
     *
     * If the copy constructor for the new value may throw, a temporary copy of
     * the old value is move- or copy-constructed before the destruction so
     * that, if the constructor for the new value throws, we can restore the
     * variant to the original value.
     *
     * Propagates any exception thrown by the assignment operator or move/copy
     * constructor.
     *
     * Requirements: All the contained types must be copy-constructible and
     * copy-assignable.
     */
    copy_assignable_variant &operator=(const copy_assignable_variant &v)
            noexcept(base::is_nothrow_copy_assignable) {
        if (this->tag() == v.tag())
            v.apply(make_assigner(this->value()));
        else
            this->emplace_with_backup(v);
        return *this;
    }

}; // template<typename... T> class copy_assignable_variant

/**
 * Either copy-assignable or conditionally move-assignable variant class,
 * selected by copy-constructibility and -assignability.
 */
template<typename... T>
using conditionally_copy_assignable_variant =
        typename std::conditional<
                for_all<std::is_copy_constructible<T>::value...>::value &&
                for_all<std::is_copy_assignable<T>::value...>::value,
                copy_assignable_variant<T...>,
                conditionally_move_assignable_variant<T...>
        >::type;

/**
 * A variant object contains exactly one object of one of the parameter types.
 * For example, an object of <code>variant&lt;int, double></code> contains
 * either an integer or double value.
 *
 * The parameter types are usually different from each other. It is allowed to
 * create a variant that has the same parameter types such as
 * <code>variant&lt;int, int></code>, but those types are indistinguishable
 * from outside of the variant object.
 *
 * @tparam T Types of values that may be contained in the variant. They all
 * must be no-throw destructible and decayed.
 */
template<typename... T>
class variant : public conditionally_copy_assignable_variant<T...> {

    using base = conditionally_copy_assignable_variant<T...>;

    // constructor inheritance
    using base::base;

public:

    static_assert(
            variant::is_nothrow_copy_constructible ==
                std::is_nothrow_copy_constructible<base>::value,
            "is_nothrow_copy_constructible is wrong");
    static_assert(
            variant::is_nothrow_move_constructible ==
                std::is_nothrow_move_constructible<base>::value,
            "is_nothrow_move_constructible is wrong");

    /**
     * Copy-assigns to this variant.
     *
     * This is effectively equivalent to converting the argument from
     * <code>variant&lt;U...></code> to <code>variant&lt;T...></code> and then
     * assigning it to this variant with the (non-template) assignment
     * operator, but this operator template uses less temporary objects.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * <code>variant&lt;U...></code> must be copy-assignable.
     */
    template<typename... U>
    typename std::enable_if<
            std::is_copy_assignable<
                    conditionally_copy_assignable_variant<U...>>::value,
            variant &
            >::type
    operator=(const variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_copy_assignable &&
                    variant::is_nothrow_move_constructible) {
        if (this->tag() == typename base::tag_type(v.tag()))
            v.apply(make_assigner(this->value()));
        else
            this->emplace_with_backup(
                    static_cast<const variant_base<U...> &>(v));
        return *this;
    }

    /**
     * Move-assigns to this variant.
     *
     * This is effectively equivalent to converting the argument from
     * <code>variant&lt;U...></code> to <code>variant&lt;T...></code> and then
     * assigning it to this variant with the (non-template) assignment
     * operator, but this operator template uses less temporary objects.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * <code>variant&lt;U...></code> must be move-assignable.
     */
    template<typename... U>
    typename std::enable_if<
            std::is_move_assignable<
                    conditionally_copy_assignable_variant<U...>>::value,
            variant &
            >::type
    operator=(variant<U...> &&v)
            noexcept(variant<U...>::is_nothrow_move_assignable &&
                    variant::is_nothrow_move_constructible) {
        if (this->tag() == typename base::tag_type(v.tag()))
            std::move(v).apply(make_assigner(this->value()));
        else
            this->emplace_with_backup(static_cast<variant_base<U...> &&>(v));
        return *this;
    }

    /**
     * Swap the values of this and the argument variants.
     *
     * If the two variants have values of the same type, they are swapped by
     * the swap function. Otherwise, the variants are swapped by emplacement
     * using the move constructor.
     *
     * Requirements: All the contained types must be swappable, no-throw
     * move-constructible.
     */
    void swap(variant &other) noexcept(variant::is_nothrow_swappable) {
        if (this->tag() == other.tag())
            return this->apply(swap_impl::swapper<variant<T...>>(other));

        assert(this != &other);

        variant<T...> temporary(std::move(*this));
        this->emplace(std::move(other));
        other.emplace(std::move(temporary));
    }

    /**
     * Converts the value of this variant to another value, which is typically
     * another variant.
     *
     * The argument is expected to be of a callable type. If it is callable
     * with the currently contained value of this variant, it is called.
     * Otherwise, the result is just the contained value. In either case, the
     * result is implicitly converted to @c R before returned.
     */
    template<
            typename F,
            typename R = typename partial_common_result<F, const T &...>::type>
    constexpr R flat_map(F &&f) const & {
        return this->apply(mapper<F, R>(std::forward<F>(f)));
    }

    /**
     * Converts the value of this variant to another value, which is typically
     * another variant.
     *
     * The argument is expected to be of a callable type. If it is callable
     * with the currently contained value of this variant, it is called.
     * Otherwise, the result is just the contained value. In either case, the
     * result is implicitly converted to @c R before returned.
     */
    template<
            typename F,
            typename R = typename partial_common_result<F, T &&...>::type>
    /* constexpr */ R flat_map(F &&f) && {
        return std::move(*this).apply(mapper<F, R>(std::forward<F>(f)));
    }

    /**
     * Converts the value of this variant by applying the argument function to
     * produce another variant with possibly different contained types.
     *
     * The argument is expected to be of a callable type. If it is callable
     * with the currently contained value of this variant, it is called.
     * Otherwise, the result is just the contained value. In either case, the
     * result is implicitly converted to @c R before returned.
     */
    template<
            typename F,
            typename R = variant<typename std::decay<typename std::result_of<
                    map_result<F>(const T &)>::type>::type...>>
    constexpr R map(F &&f) const & {
        return this->apply(mapper<F, R>(std::forward<F>(f)));
    }

    /**
     * Converts the value of this variant by applying the argument function to
     * produce another variant with possibly different contained types.
     *
     * The argument is expected to be of a callable type. If it is callable
     * with the currently contained value of this variant, it is called.
     * Otherwise, the result is just the contained value. In either case, the
     * result is implicitly converted to @c R before returned.
     */
    template<
            typename F,
            typename R = variant<typename std::decay<typename std::result_of<
                    map_result<F>(T &&)>::type>::type...>>
    /* constexpr */ R map(F &&f) && {
        return std::move(*this).apply(mapper<F, R>(std::forward<F>(f)));
    }

    /**
     * Creates a new variant by constructing its contained value by calling the
     * constructor of <code>U</code> with forwarded arguments.
     *
     * Propagates any exception thrown by the constructor.
     *
     * @tparam U the type of the new contained value to be constructed.
     * @tparam Arg the type of constructor arguments.
     * @param arg arguments forwarded to the constructor.
     */
    template<typename U, typename... Arg>
    static variant create(Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) {
        return variant(
                direct_initialize(), type_tag<U>(), std::forward<Arg>(arg)...);
    }

    /**
     * Creates a new variant by constructing its contained value by calling the
     * constructor of <code>U</code> with forwarded initializer list and
     * arguments.
     *
     * Propagates any exception thrown by the constructor.
     *
     * @tparam U the type of the new contained value to be constructed.
     * @tparam ListArg the type of the initializer list items.
     * @tparam Arg the type of constructor arguments.
     * @param list initializer list forwarded to the constructor.
     * @param arg arguments forwarded to the constructor.
     */
    template<typename U, typename ListArg, typename... Arg>
    static
    typename std::enable_if<
            std::is_constructible<
                    U, std::initializer_list<ListArg> &, Arg &&...>::value,
            variant>::type
    create(std::initializer_list<ListArg> list, Arg &&... arg)
            noexcept(
                std::is_nothrow_constructible<
                    U, std::initializer_list<ListArg> &, Arg &&...>::value) {
        return variant(
                direct_initialize(),
                type_tag<U>(),
                list,
                std::forward<Arg>(arg)...);
    }

}; // template<typename... T> class variant

/**
 * Swaps two variants.
 *
 * If the two variants have values of the same type, they are swapped by the
 * swap function. Otherwise, the variants are swapped by emplacement using the
 * move constructor.
 *
 * Requirements: All the contained types must be swappable, no-throw
 * move-constructible.
 */
template<typename... T>
void swap(variant<T...> &a, variant<T...> &b)
        noexcept(variant<T...>::is_nothrow_swappable) {
    a.swap(b);
}

template<template<typename> class C, typename... T>
class comparator {

private:

    const variant<T...> &m_variant;

public:

    constexpr explicit comparator(const variant<T...> &v) noexcept :
            m_variant(v) { }

    template<typename U>
    constexpr bool operator()(const U &u) const {
        return C<U>()(u, m_variant.template value<U>());
    }

}; // template<template<typename> typename C, typename... T> class comparator

/** Checks if two variants have the same indexes and equal values. */
template<typename... T>
constexpr bool operator==(const variant<T...> &l, const variant<T...> &r) {
    return l.tag() == r.tag() && l.apply(comparator<std::equal_to, T...>(r));
}

/**
 * Checks the order of two variants.
 *
 * Variants with different type indexes are ordered by indexes. Variants with
 * the same indexes are compared by applying the <code>&lt;</code> operator to
 * the values.
 */
template<typename... T>
constexpr bool operator<(const variant<T...> &l, const variant<T...> &r) {
    return l.tag() == r.tag() ?
            l.apply(comparator<std::less, T...>(r)) :
            l.tag() < r.tag();
}

} // namespace variant_impl

using variant_impl::variant;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_variant_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
