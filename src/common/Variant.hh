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

#ifndef INCLUDED_common_Variant_hh
#define INCLUDED_common_Variant_hh

#include "buildconfig.h"

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include "common/FunctionalInitialize.hh"

namespace sesh {
namespace common {

namespace variant_impl {

/**
 * Defined to be (a subclass of) std::true_type or std::false_type depending on
 * the template parameter types. The Boolean will be true if and only if
 * <code>Predicate&lt;T>::value</code> is true for all <code>T</code>s.
 */
template<template<typename> class Predicate, typename... T>
class ForAll;

template<template<typename> class Predicate>
class ForAll<Predicate> : public std::true_type { };

template<template<typename> class Predicate, typename Head, typename... Tail>
class ForAll<Predicate, Head, Tail...> :
        public std::conditional<
                Predicate<Head>::value,
                ForAll<Predicate, Tail...>,
                std::false_type>::type { };

/**
 * Defined to be (a subclass of) std::true_type or std::false_type depending on
 * the template parameter types. The Boolean will be true if and only if
 * {@code T} is the same type as one (or more) of {@code U}s.
 */
template<typename T, typename... U>
class IsAnyOf;

template<typename T>
class IsAnyOf<T> : public std::false_type { };

template<typename T, typename Head, typename... Tail>
class IsAnyOf<T, Head, Tail...> :
        public std::conditional<
                std::is_same<T, Head>::value,
                std::true_type,
                IsAnyOf<T, Tail...>>::type { };

/** Integral type that identifies the type of the active value of a variant. */
using Index = unsigned;

/** Contains the value of a variant. */
template<Index, typename...>
union Union;

template<Index headIndex>
union Union<headIndex> {

public:

    /** Must never be called. */
    template<Index, typename...>
    constexpr static Index convertIndex(Index i) noexcept {
        // assert(false); not allowed in constexpr function
        return i;
    }

    /** Must never be called. */
    template<typename Visitor, typename Result>
    Result apply(Index, Visitor &&) const noexcept {
        assert(false);
        throw nullptr;
    }

};

template<Index headIndex, typename Head, typename... Tail>
union Union<headIndex, Head, Tail...> {

    static_assert(
            std::is_same<Head, typename std::decay<Head>::type>::value,
            "Contained type must be decayed");

private:

    using TailUnion = Union<headIndex + 1u, Tail...>;

    Head mHead;
    TailUnion mTail;

public:

    /** Returns the index of type <code>U</code>. */
    template<typename U>
    constexpr static
    typename std::enable_if<std::is_same<U, Head>::value, Index>::type
    index() noexcept {
        return headIndex;
    }

    /** Returns the index for the template parameter type. */
    template<typename U>
    constexpr static
    typename std::enable_if<!std::is_same<U, Head>::value, Index>::type
    index() noexcept {
        return TailUnion::template index<U>();
    }

    /**
     * Converts the type index for this union into the index of the same type
     * in <code>Variant&lt;U...></code>. All the types that may be contained in
     * this union (<code>T...</code>) must be included in the template
     * parameter types (<code>U...</code>).
     */
    template<Index i, typename... U>
    constexpr static Index convertIndex(Index index) noexcept {
        return (index == headIndex) ?
                Union<i, U...>::template index<Head>() :
                TailUnion::template convertIndex<i, U...>(index);
    }

    /** The constructor does nothing. */
    Union() noexcept { }

    /** Union cannot be copied without knowing the current value's type. */
    Union(const Union &) = delete;

    /** Union cannot be moved without knowing the current value's type. */
    Union(Union &&) = delete;

    /**
     * The destructor does nothing. The active value in this union, if any,
     * must be explicitly destructed using {@link Destructor} before this
     * destructor is called.
     */
    ~Union() noexcept { }

    /** Union cannot be assigned without knowing the current value's type. */
    Union &operator=(const Union &) = delete;

    /** Union cannot be assigned without knowing the current value's type. */
    Union &operator=(Union &&) = delete;

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, U &>::type
    value() noexcept {
        return mHead;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, const U &>::type
    value() const noexcept {
        return mHead;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, U &>::type
    value() noexcept {
        return mTail.template value<U>();
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, const U &>::type
    value() const noexcept {
        return mTail.template value<U>();
    }

    /**
     * Constructs the value of type <code>U</code> using the given arguments.
     * This function must not be called when a value of this union has already
     * been constructed for some type.
     */
    template<typename U, typename... Arg>
    void construct(Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) {
        new (std::addressof(value<U>())) U(std::forward<Arg>(arg)...);
    }

    /**
     * Constructs the value by move-constructing the result of the argument
     * function, which must return (a reference to) a value of one of the types
     * that may be contained in this union. This function must not be called
     * when a value of this union has already been constructed for some type.
     */
    template<typename F, typename U>
    void constructFrom(F &&f) {
        new (std::addressof(value<U>())) U(std::forward<F>(f)());
    }

    /**
     * Calls the argument visitor's () operator with the value of the argument
     * index. The value is passed by l-value reference to the () operator.
     */
    template<typename Visitor, typename Result>
    Result apply(Index index, Visitor &&visitor) &
            noexcept(noexcept(
                    index == headIndex ?
                    std::forward<Visitor>(visitor)(mHead) :
                    mTail.template apply<Visitor, Result>(
                            index, std::forward<Visitor>(visitor)))) {
        if (index == headIndex)
            return std::forward<Visitor>(visitor)(mHead);
        else
            return mTail.template apply<Visitor, Result>(
                    index, std::forward<Visitor>(visitor));
    }

    /**
     * Calls the argument visitor's () operator with the value of the argument
     * index. The value is passed by const l-value reference to the ()
     * operator.
     */
    template<typename Visitor, typename Result>
    Result apply(Index index, Visitor &&visitor) const &
            noexcept(noexcept(
                    index == headIndex ?
                    std::forward<Visitor>(visitor)(mHead) :
                    mTail.template apply<Visitor, Result>(
                            index, std::forward<Visitor>(visitor)))) {
        if (index == headIndex)
            return std::forward<Visitor>(visitor)(mHead);
        else
            return mTail.template apply<Visitor, Result>(
                    index, std::forward<Visitor>(visitor));
    }

    /**
     * Calls the argument visitor's () operator with the value of the argument
     * index. The value is passed by r-value reference to the () operator.
     */
    template<typename Visitor, typename Result>
    Result apply(Index index, Visitor &&visitor) &&
            noexcept(noexcept(
                    index == headIndex ?
                    std::forward<Visitor>(visitor)(std::move(mHead)) :
                    std::move(mTail).template apply<Visitor, Result>(
                            index, std::forward<Visitor>(visitor)))) {
        if (index == headIndex)
            return std::forward<Visitor>(visitor)(std::move(mHead));
        else
            return std::move(mTail).template apply<Visitor, Result>(
                    index, std::forward<Visitor>(visitor));
    }

};

/**
 * A type tag is a dummy object that is passed to a variant constructor to tell
 * the type of the object to be created and contained in the variant.
 *
 * @tparam T type of the object to be created and contained in the variant.
 */
template<typename T>
class TypeTag { };

/** A visitor that constructs a value of a target union. */
template<typename Union>
class Constructor {

private:

    Union &mTarget;

public:

    Constructor(Union &target) noexcept : mTarget(target) { }

    using Result = void;

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v)
            noexcept(std::is_nothrow_constructible<V, T &&>::value) {
        mTarget.template construct<V>(std::forward<T>(v));
    }

};

/** A visitor that assigns a value to a target union. */
template<typename Union>
class Assigner {

private:

    Union &mTarget;

public:

    Assigner(Union &target) noexcept : mTarget(target) { }

    using Result = void;

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v)
            noexcept(std::is_nothrow_assignable<V, T &&>::value) {
        mTarget.template value<V>() = std::forward<T>(v);
    }

};

/** A visitor that destructs a union value. */
class Destructor {

public:

    using Result = void;

    template<typename V>
    void operator()(V &v) noexcept(std::is_nothrow_destructible<V>::value) {
        v.~V();
    }

};

/** A visitor that emplaces the target variant. */
template<typename Variant>
class Emplacer {

private:

    Variant &mTarget;

public:

    Emplacer(Variant &target) noexcept : mTarget(target) { }

    using Result = void;

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v)
            noexcept(noexcept(std::declval<Variant>().template emplace<V>(
                    std::forward<T>(v)))) {
        mTarget.template emplace<V>(std::forward<T>(v));
    }

};

namespace {

/** A helper function that constructs a constructor. */
template<typename Union>
Constructor<Union> constructor(Union &target) noexcept {
    return Constructor<Union>(target);
}

/** A helper function that constructs an assigner. */
template<typename Union>
Assigner<Union> assigner(Union &target) noexcept {
    return Assigner<Union>(target);
}

/** A helper function that constructs an emplacer. */
template<typename Variant>
Emplacer<Variant> emplacer(Variant &variant) noexcept {
    return Emplacer<Variant>(variant);
}

} // namespace

namespace swap_impl {

using std::swap;

template<typename T>
class IsSwappable {

private:

    template<typename U>
    static auto f(U &) -> decltype(
            (void) swap(std::declval<U &>(), std::declval<U &>()),
            std::true_type());

    template<typename>
    static auto f(...) -> std::false_type;

public:

    using type = decltype(f<T>(std::declval<T &>()));

}; // template<typename T> class IsSwappable

template<typename T, bool = IsSwappable<T>::type::value>
class IsNothrowSwappableImpl {

public:

    using type = std::false_type;

}; // template<typename, bool> class IsNothrowSwappableImpl

template<typename T>
class IsNothrowSwappableImpl<T, true> {

public:

    using type = std::integral_constant<
            bool,
            noexcept(swap(std::declval<T &>(), std::declval<T &>()))>;

}; // template<typename T> class IsNothrowSwappableImpl<T, true>

template<typename T>
using IsNothrowSwappable = typename IsNothrowSwappableImpl<T>::type;

template<typename Variant>
class Swapper {

private:

    Variant &mOther;

public:

    Swapper(Variant &other) noexcept : mOther(other) { }

    using Result = void;

    template<typename T>
    void operator()(T &v) noexcept(IsNothrowSwappable<T>::value) {
        swap(v, mOther.template value<T>());
    }

};

} // namespace swap_impl

/** Fundamental implementation of variant. */
template<typename... T>
class VariantBase {

private:

    constexpr static Index INDEX_BASE = 0u;
    using Value = Union<INDEX_BASE, T...>;

    Index mIndex;
    Value mValue;

protected:

    Value &value() noexcept { return mValue; }
    const Value &value() const noexcept { return mValue; }

public:

    /** The nth type of the contained types. */
    template<std::size_t n>
    using NthType = typename std::tuple_element<n, std::tuple<T...>>::type;

    constexpr static bool IS_NOTHROW_COPY_CONSTRUCTIBLE =
            ForAll<std::is_nothrow_copy_constructible, T...>::value;
    constexpr static bool IS_NOTHROW_MOVE_CONSTRUCTIBLE =
            ForAll<std::is_nothrow_move_constructible, T...>::value;
    constexpr static bool IS_NOTHROW_DESTRUCTIBLE =
            ForAll<std::is_nothrow_destructible, T...>::value;
    constexpr static bool IS_NOTHROW_COPY_ASSIGNABLE =
            ForAll<std::is_nothrow_copy_assignable, T...>::value &&
            IS_NOTHROW_COPY_CONSTRUCTIBLE &&
            IS_NOTHROW_DESTRUCTIBLE;
    constexpr static bool IS_NOTHROW_MOVE_ASSIGNABLE =
            ForAll<std::is_nothrow_move_assignable, T...>::value &&
            IS_NOTHROW_MOVE_CONSTRUCTIBLE &&
            IS_NOTHROW_DESTRUCTIBLE;
    constexpr static bool IS_NOTHROW_SWAPPABLE =
            ForAll<swap_impl::IsNothrowSwappable, T...>::value;

    /** Returns the integral value that identifies the parameter type. */
    template<typename U>
    constexpr static Index index() noexcept {
        return Value::template index<U>();
    }

    /**
     * Returns an integral value that identifies the type of the currently
     * contained value.
     */
    Index index() const noexcept { return mIndex; }

    /**
     * Converts the index of the currently contained type in this variant into
     * the index of the same type in <code>Variant&lt;U...></code>. All the
     * types that may be contained in this variant (<code>T...</code>) must be
     * included in the template parameter types (<code>U...</code>).
     */
    template<typename... U>
    Index convertIndex() const noexcept {
        return Value::template convertIndex<INDEX_BASE, U...>(index());
    }

    /**
     * Creates a new variant by constructing its contained value by calling the
     * constructor of <code>U</code> with forwarded arguments.
     *
     * Throws any exception thrown by the constructor.
     *
     * @tparam U the type of the new contained value to be constructed.
     *     (inferred from the type of type tag argument.)
     * @tparam Arg the type of the constructor's arguments.
     * @param arg the arguments forwarded to the constructor.
     */
    template<typename U, typename... Arg>
    VariantBase(TypeTag<U>, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) :
            mIndex(index<U>()) {
        value().template construct<U>(std::forward<Arg>(arg)...);
    }

    /**
     * Creates a new variant by copy- or move-constructing its contained value
     * from the argument.
     *
     * Throws any exception thrown by the constructor.
     *
     * @tparam U the argument type.
     * @tparam V the type of the new contained value to be constructed.
     *     (inferred from the argument type.)
     * @param v the argument forwarded to the constructor.
     */
    template<
            typename U,
            typename V = typename std::decay<U>::type,
            typename = typename std::enable_if<IsAnyOf<V, T...>::value>::type>
    VariantBase(U &&v)
            noexcept(std::is_nothrow_constructible<V, U &&>::value) :
            VariantBase(TypeTag<V>(), std::forward<U>(v)) { }

    /**
     * Creates a new variant by move-constructing its contained value from the
     * result of calling the argument function.
     *
     * Throws any exception thrown by the argument function or constructor.
     *
     * @tparam F the type of the function argument.
     * @tparam U the type of the new contained value to be constructed.
     *     (inferred from the return type of the argument function.)
     * @param f the function that constructs the new contained value.
     */
    template<
            typename F,
            typename U = typename std::decay<
                    typename std::result_of<F()>::type>::type>
    VariantBase(FunctionalInitialize, F &&f) : mIndex(index<U>()) {
        value().template constructFrom<F, U>(std::forward<F>(f));
    }

    /**
     * Returns a reference to the value of the template parameter type.
     *
     * This function can be called only when the currently contained value is
     * of the parameter type. The referenced object's lifetime ends when the
     * type of the contained value is changed or the variant is destructed.
     */
    template<typename U>
    U &value() & noexcept {
        assert(index() == index<U>());
        return mValue.template value<U>();
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
        assert(index() == index<U>());
        return mValue.template value<U>();
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
        assert(index() == index<U>());
        return std::move(mValue.template value<U>());
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
     * @tparam Result the return type of the () operators.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<
            typename Visitor,
            typename Result =
                    typename std::remove_reference<Visitor>::type::Result>
    Result apply(Visitor &&visitor) &
            noexcept(noexcept(
                std::declval<Value &>().template apply<Visitor, Result>(
                    std::declval<Index>(),
                    std::forward<Visitor>(visitor)))) {
        return value().template apply<Visitor, Result>(
                index(), std::forward<Visitor>(visitor));
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
     * @tparam Result the return type of the () operators.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<
            typename Visitor,
            typename Result =
                    typename std::remove_reference<Visitor>::type::Result>
    Result apply(Visitor &&visitor) const &
            noexcept(noexcept(
                std::declval<const Value &>().template apply<Visitor, Result>(
                    std::declval<Index>(),
                    std::forward<Visitor>(visitor)))) {
        return value().template apply<Visitor, Result>(
                index(), std::forward<Visitor>(visitor));
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
     * @tparam Result the return type of the () operators.
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<
            typename Visitor,
            typename Result =
                    typename std::remove_reference<Visitor>::type::Result>
    Result apply(Visitor &&visitor) &&
            noexcept(noexcept(
                std::declval<Value &&>().template apply<Visitor, Result>(
                    std::declval<Index>(),
                    std::forward<Visitor>(visitor)))) {
        return std::move(value()).template apply<Visitor, Result>(
                index(), std::forward<Visitor>(visitor));
    }

    /**
     * Destructor.
     *
     * Calls the currently contained value's destructor.
     *
     * Propagates any exception thrown by the destructor.
     */
    ~VariantBase() noexcept(IS_NOTHROW_DESTRUCTIBLE) {
        apply(Destructor());
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
    VariantBase(const VariantBase &v) noexcept(IS_NOTHROW_COPY_CONSTRUCTIBLE) :
            mIndex(v.index()) {
        v.apply(constructor(value()));
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
    VariantBase(const VariantBase<U...> &v)
            noexcept(VariantBase<U...>::IS_NOTHROW_COPY_CONSTRUCTIBLE) :
            mIndex(v.template convertIndex<T...>()) {
        v.apply(constructor(value()));
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
    VariantBase(VariantBase &&v) noexcept(IS_NOTHROW_MOVE_CONSTRUCTIBLE) :
            mIndex(v.index()) {
        std::move(v).apply(constructor(value()));
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
    VariantBase(VariantBase<U...> &&v)
            noexcept(VariantBase<U...>::IS_NOTHROW_MOVE_CONSTRUCTIBLE) :
            mIndex(v.template convertIndex<T...>()) {
        std::move(v).apply(constructor(this->value()));
    }

private:

    /**
     * Re-constructs this variant by forwarding the argument to the applicable
     * constructor of <code>U</code>. This function assumes the constructor
     * never throws. If the constructor did throw something at runtime,
     * std::terminate is called.
     */
    template<typename U, typename... Arg>
    void reconstructOrTerminate(Arg &&... arg) noexcept {
        new (this) VariantBase(TypeTag<U>(), std::forward<Arg>(arg)...);
    }

public:

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of type <code>U</code> with the given
     * arguments.
     *
     * To ensure that a valid object is contained after return from this
     * function, nothing must be thrown by the destructor that destructs the
     * currently contained value or the constructor of the new value. If
     * something is thrown at runtime, std::terminate is called.
     *
     * @see #emplaceWithFallback
     * @see #reset
     */
    template<typename U, typename... Arg>
    void emplace(Arg &&... arg) noexcept {
        this->~VariantBase();
        new (this) VariantBase(TypeTag<U>(), std::forward<Arg>(arg)...);
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of type <code>U</code> with the given
     * arguments.
     *
     * If the destructor or constructor threw something, the default
     * constructor of <code>Fallback</code> is called as a fallback and the
     * exception is re-thrown. If the <code>Fallback</code> default constructor
     * threw again, std::terminate is called.
     */
    template<typename U, typename Fallback, typename... Arg>
    void emplaceWithFallback(Arg &&... arg)
            noexcept(IS_NOTHROW_DESTRUCTIBLE &&
                    std::is_nothrow_constructible<U, Arg &&...>::value) {
        try {
            this->~VariantBase();
            new (this) VariantBase(TypeTag<U>(), std::forward<Arg>(arg)...);
        } catch (...) {
            reconstructOrTerminate<Fallback>();
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
        emplace<V>(std::forward<U>(v));
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
    void assign(U &&v) noexcept(std::is_nothrow_assignable<V, U &&>::value) {
        if (index() == index<V>())
            value<V>() = std::forward<U>(v);
        else
            emplace<V>(std::forward<U>(v));
    }

    /**
     * Assignment from the same type is disabled by default. Implementation
     * subclasses may have their own assignment operators.
     */
    VariantBase &operator=(const VariantBase &) = delete;

    /**
     * Assignment from the same type is disabled by default. Implementation
     * subclasses may have their own assignment operators.
     */
    VariantBase &operator=(VariantBase &&) = delete;

};

/**
 * A subclass of variant base that re-defines copy and move assignment
 * operators. Actual assignment operators may not be defined if contained types
 * do not have copy/move constructor/assignment operator.
 */
template<typename... T>
class AssignableVariant : public VariantBase<T...> {

private:

    using Base = VariantBase<T...>;

    static_assert(Base::IS_NOTHROW_MOVE_CONSTRUCTIBLE,
            "Variable assignment requires no-throw move");
    static_assert(Base::IS_NOTHROW_DESTRUCTIBLE,
            "Variable assignment requires no-throw destructor");

public:

    using Base::Base;

    AssignableVariant(const AssignableVariant &) = default;
    AssignableVariant(AssignableVariant &&) = default;

    /**
     * Copy assignment operator.
     *
     * If the left-hand-side and right-hand-side contain values of the same
     * type, the value is assigned by its copy assignment operator. Otherwise,
     * the old value is destructed and the new value is copy-constructed.
     *
     * If the copy constructor for the new value may throw, a temporary copy of
     * the old value is move-constructed before the destruction so that, if the
     * constructor for the new value throws, we can restore the variant to the
     * original value. The move constructor may not throw in this case.
     *
     * Propagates any exception thrown by the assignment operator or the copy
     * constructor.
     *
     * Requirements: All the contained types must be copy-constructible,
     * no-throw move-constructible, copy-assignable, and no-throw destructible.
     */
    AssignableVariant &operator=(const AssignableVariant &v)
            noexcept(Base::IS_NOTHROW_COPY_ASSIGNABLE) {
        if (this->index() == v.index())
            v.apply(assigner(this->value()));
        else
            v.apply(emplacer(*this));
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
     * Propagates any exception thrown by the assignment operator.
     *
     * Requirements: All the contained types must be no-throw
     * move-constructible, move-assignable, and no-throw destructible.
     */
    AssignableVariant &operator=(AssignableVariant &&v)
            noexcept(Base::IS_NOTHROW_MOVE_ASSIGNABLE) {
        if (this->index() == v.index())
            std::move(v).apply(assigner(this->value()));
        else
            std::move(v).apply(emplacer(*this));
        return *this;
    };

};

/**
 * Either assignable variant or variant base class, selected by possibility of
 * exceptions in assignment. Note that not all assignable variants have
 * assignment operators.
 */
template<typename... T>
using ConditionallyAssignableVariant =
        typename std::conditional<
                VariantBase<T...>::IS_NOTHROW_MOVE_CONSTRUCTIBLE &&
                        VariantBase<T...>::IS_NOTHROW_DESTRUCTIBLE,
                AssignableVariant<T...>,
                VariantBase<T...>
        >::type;

/**
 * A variant object contains exactly one object of one of the parameter types.
 * For example, an object of <code>Variant&lt;int, double></code> contains
 * either an integer or double value.
 *
 * The parameter types are usually different from each other. It is allowed to
 * create a variant that has the same parameter types such as
 * <code>Variant&lt;int, int></code>, but those types are indistinguishable
 * from outside of the variant object.
 *
 * @tparam T the types of which the value of this variant may be. They must be
 * decayed types (see std::decay).
 */
template<typename... T>
class Variant : public ConditionallyAssignableVariant<T...> {

    using Base = ConditionallyAssignableVariant<T...>;

    // constructor inheritance
    using Base::Base;

public:

    static_assert(
            Variant::IS_NOTHROW_COPY_CONSTRUCTIBLE ==
                std::is_nothrow_copy_constructible<Base>::value,
            "IS_NOTHROW_COPY_CONSTRUCTIBLE is wrong");
    static_assert(
            Variant::IS_NOTHROW_MOVE_CONSTRUCTIBLE ==
                std::is_nothrow_move_constructible<Base>::value,
            "IS_NOTHROW_MOVE_CONSTRUCTIBLE is wrong");
    static_assert(
            Variant::IS_NOTHROW_DESTRUCTIBLE ==
                std::is_nothrow_destructible<Base>::value,
            "IS_NOTHROW_DESTRUCTIBLE is wrong");

    /**
     * Copy-assigns to this variant.
     *
     * This is effectively equivalent to converting the argument from
     * <code>Variant&lt;U...></code> to <code>Variant&lt;T...></code> and then
     * assigning it to this variant with the (non-template) assignment
     * operator, but this operator template uses less temporary objects.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * <code>Variant&lt;U...></code> must be copy-assignable.
     */
    template<typename... U>
    Variant &operator=(const Variant<U...> &v)
            noexcept(Variant<U...>::IS_NOTHROW_COPY_ASSIGNABLE) {
        if (this->index() == v.template convertIndex<T...>())
            v.apply(assigner(this->value()));
        else
            v.apply(emplacer(*this));
        return *this;
    }

    /**
     * Move-assigns to this variant.
     *
     * This is effectively equivalent to converting the argument from
     * <code>Variant&lt;U...></code> to <code>Variant&lt;T...></code> and then
     * assigning it to this variant with the (non-template) assignment
     * operator, but this operator template uses less temporary objects.
     *
     * @tparam U Types that may be contained in the argument variant. All
     * <code>U</code>s must be contained in <code>T</code>s and
     * <code>Variant&lt;U...></code> must be move-assignable.
     */
    template<typename... U>
    Variant &operator=(Variant<U...> &&v)
            noexcept(Variant<U...>::IS_NOTHROW_MOVE_ASSIGNABLE) {
        if (this->index() == v.template convertIndex<T...>())
            std::move(v).apply(assigner(this->value()));
        else
            std::move(v).apply(emplacer(*this));
        return *this;
    }

    /** Swap the values of this and the argument variant. */
    void swap(Variant &other) noexcept(Variant::IS_NOTHROW_SWAPPABLE);

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
    static Variant create(Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value &&
                    std::is_nothrow_destructible<U>::value) {
        return Variant(TypeTag<U>(), std::forward<Arg>(arg)...);
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
            Variant>::type
    create(std::initializer_list<ListArg> list, Arg &&... arg)
            noexcept(
                std::is_nothrow_constructible<
                    U, std::initializer_list<ListArg> &, Arg &&...>::value &&
                std::is_nothrow_destructible<U>::value) {
        return Variant(TypeTag<U>(), list, std::forward<Arg>(arg)...);
    }

    /**
     * Creates a new variant by move-constructing its contained value from the
     * result of calling the argument function.
     *
     * Throws any exception thrown by the argument function or constructor.
     *
     * @tparam F the type of the function argument.
     * @tparam U the type of the new contained value to be constructed.
     *     (inferred from the return type of the argument function.)
     * @param f the function that constructs the new contained value.
     */
    template<
            typename F,
            typename U = typename std::decay<
                    typename std::result_of<F()>::type>::type>
    static Variant resultOf(F &&f) {
        return Variant(FUNCTIONAL_INITIALIZE, std::forward<F>(f));
    }

};

/**
 * Swaps two variants.
 *
 * If the two variants have values of the same type, they are swapped by the
 * swap function. Otherwise, the variants are swapped by emplacement using the
 * move constructor.
 *
 * Requirements: All the contained types must be swappable, copy-constructible,
 * and no-throw move-constructible.
 */
template<typename... T>
void swap(Variant<T...> &a, Variant<T...> &b)
        noexcept(Variant<T...>::IS_NOTHROW_SWAPPABLE &&
                Variant<T...>::IS_NOTHROW_COPY_CONSTRUCTIBLE &&
                Variant<T...>::IS_NOTHROW_DESTRUCTIBLE) {
    if (a.index() == b.index())
        return a.apply(swap_impl::Swapper<Variant<T...>>(b));

    assert(&a != &b);

    Variant<T...> temporary(std::move(a));
    std::move(b).apply(emplacer(a));
    std::move(temporary).apply(emplacer(b));
}

template<typename... T>
void Variant<T...>::swap(Variant &other)
        noexcept(Variant::IS_NOTHROW_SWAPPABLE) {
    variant_impl::swap(*this, other);
}

/*
 * XXX: A more efficient specialization for a single contained type variant may
 * be defined because its index is always zero.
 */

} // namespace variant_impl

using variant_impl::Variant;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_Variant_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
