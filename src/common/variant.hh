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
#include "common/tagged_union.hh"
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
class variant {

public:

    /** Internal type to contain the variant value. */
    using value_type = tagged_union<T...>;

    /**
     * The type of the type tag which specifies the type of the contained
     * value.
     */
    using tag_type = typename value_type::tag_type;

private:

    value_type m_value;

public:

    /** Returns the reference to the internal tagged union. */
    value_type &value() noexcept { return m_value; }
    /** Returns the reference to the internal tagged union. */
    const value_type &value() const noexcept { return m_value; }

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
    tag_type tag() const noexcept { return m_value.tag(); }

    /**
     * Creates a new variant by calling the constructor of the backing tagged
     * union with the arguments forwarded to it.
     *
     * Throws any exception thrown by the constructor.
     *
     * @param h First argument forwarded to the tagged union constructor.
     * @param t Other arguments forwarded to the tagged union constructor.
     * @see tagged_union
     */
    template<typename Head, typename... Tail>
    variant(direct_initialize, Head &&h, Tail &&... t)
            noexcept(std::is_nothrow_constructible<
                    value_type, Head, Tail...>::value) :
            m_value(std::forward<Head>(h), std::forward<Tail>(t)...) { }

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
    variant(U &&v)
            noexcept(std::is_nothrow_constructible<V, U &&>::value) :
            variant(direct_initialize(), type_tag<V>(), std::forward<U>(v)) { }

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
    variant(const variant &v) = default;

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
    variant(const variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_copy_constructible) :
            variant(direct_initialize(), v.value()) { }

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
    variant(variant &&v) = default;

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
    variant(variant<U...> &&v)
            noexcept(variant<U...>::is_nothrow_move_constructible) :
            variant(direct_initialize(), std::move(v.value())) { }

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
    variant(move_if_noexcept min, variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_move_constructible) :
            m_value(min, v.value()) { }

    /**
     * Destructor.
     *
     * Calls the currently contained value's destructor.
     */
    ~variant() = default;

    /**
     * Returns a reference to the value of the template parameter type.
     *
     * This function can be called only when the currently contained value is
     * of the parameter type. The referenced object's lifetime ends when the
     * type of the contained value is changed or the variant is destructed.
     */
    template<typename U>
    U &value() & noexcept {
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
            noexcept(noexcept(std::declval<value_type &>().apply(
                    std::declval<Visitor>())))
            -> typename common_result<Visitor, T &...>::type {
        return m_value.apply(std::forward<Visitor>(visitor));
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
            noexcept(noexcept(std::declval<const value_type &>().apply(
                    std::declval<Visitor>())))
            -> typename common_result<Visitor, const T &...>::type {
        return m_value.apply(std::forward<Visitor>(visitor));
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
            noexcept(noexcept(std::declval<value_type &&>().apply(
                    std::declval<Visitor>())))
            -> typename common_result<Visitor, T &&...>::type {
        return std::move(m_value).apply(std::forward<Visitor>(visitor));
    }

private:

    /*
     * Lverloads of construct_value have the same signatures as that of the
     * variant constructor.
     */

    template<typename Head, typename... Tail>
    void construct_value(direct_initialize, Head &&h, Tail &&... t)
            noexcept(std::is_nothrow_constructible<
                    value_type, Head, Tail...>::value) {
        new (&m_value) value_type(
                std::forward<Head>(h), std::forward<Tail>(t)...);
    }

    template<
            typename U,
            typename V = typename std::decay<U>::type,
            typename = typename std::enable_if<
                    std::is_constructible<V, U>::value>::type,
            typename = typename std::enable_if<
                    is_any_of<V, T...>::value>::type>
    void construct_value(U &&v)
            noexcept(std::is_nothrow_constructible<V, U &&>::value) {
        return construct_value(
                direct_initialize(), type_tag<V>(), std::forward<U>(v));
    }

    template<typename... U>
    void construct_value(const variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_copy_constructible) {
        return construct_value(direct_initialize(), v.value());
    }

    template<typename... U>
    void construct_value(variant<U...> &&v)
            noexcept(variant<U...>::is_nothrow_move_constructible) {
        return construct_value(direct_initialize(), std::move(v.value()));
    }

    template<typename... U>
    void construct_value(move_if_noexcept min, variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_move_constructible) {
        new (&m_value) value_type(min, v.value());
    }

    template<typename... Arg>
    void construct_value_or_terminate(Arg &&... a) noexcept {
        return construct_value(std::forward<Arg>(a)...);
    }

public:

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
     */
    template<typename... Arg>
    void emplace(Arg &&... arg) noexcept {
        m_value.~value_type();
        construct_value(std::forward<Arg>(arg)...);
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
            noexcept(std::is_nothrow_constructible<variant, Arg &&...>::value)
    {
        try {
            m_value.~value_type();
            construct_value(std::forward<Arg>(arg)...);
        } catch (...) {
            construct_value_or_terminate(
                    direct_initialize(), type_tag<Fallback>());
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
            std::is_nothrow_constructible<variant, Arg &&...>::value>::type
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
            !std::is_nothrow_constructible<variant, Arg &&...>::value>::type
    emplace_with_backup(Arg &&... arg) {
        value_type backup(move_if_noexcept(), m_value);
        try {
            m_value.~value_type();
            construct_value(std::forward<Arg>(arg)...);
        } catch (...) {
            construct_value_or_terminate(
                    direct_initialize(), std::move(backup));
            throw;
        }
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
     */
    template<typename U, typename V = typename std::decay<U>::type>
    void assign(U &&v) noexcept(std::is_nothrow_assignable<V &, U &&>::value) {
        if (tag() == tag<V>())
            value<V>() = std::forward<U>(v);
        else
            emplace(std::forward<U>(v));
    }

protected:

    /** A visitor that assigns a value to the tagged union. */
    class assigner {

    private:

        value_type &m_target;

    public:

        constexpr explicit assigner(value_type &t) noexcept : m_target(t) { }

        template<typename U, typename V = typename std::decay<U>::type>
        void operator()(U &&v) const
                noexcept(std::is_nothrow_assignable<V &, U &&>::value) {
            m_target.template value<V>() = std::forward<U>(v);
        }

    }; // class assigner

public:

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
    variant &operator=(const variant &v) noexcept(is_nothrow_copy_assignable) {
        if (tag() == v.tag())
            v.apply(assigner(value()));
        else
            emplace_with_backup(v);
        return *this;
    }

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
    variant &operator=(const variant<U...> &v)
            noexcept(variant<U...>::is_nothrow_copy_assignable &&
                    is_nothrow_move_constructible) {
        if (tag() == tag_type(v.tag()))
            v.apply(assigner(value()));
        else
            emplace_with_backup(v);
        return *this;
    }

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
    variant &operator=(variant &&v) noexcept(is_nothrow_move_assignable) {
        if (tag() == v.tag())
            std::move(v).apply(assigner(value()));
        else
            emplace_with_backup(std::move(v));
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
    variant &operator=(variant<U...> &&v)
            noexcept(variant<U...>::is_nothrow_move_assignable &&
                    is_nothrow_move_constructible) {
        if (tag() == tag_type(v.tag()))
            std::move(v).apply(assigner(value()));
        else
            emplace_with_backup(std::move(v));
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
    void swap(variant &other) noexcept(is_nothrow_swappable) {
        if (tag() == other.tag())
            return apply(swap_impl::swapper<variant<T...>>(other));

        assert(this != &other);

        variant temporary(std::move(*this));
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
        return apply(mapper<F, R>(std::forward<F>(f)));
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
        return apply(mapper<F, R>(std::forward<F>(f)));
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
