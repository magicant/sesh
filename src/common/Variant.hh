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
#include "common/CommonResult.hh"
#include "common/FunctionalInitialize.hh"
#include "common/TypeTag.hh"

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

template<typename T>
class IsTypeTag : public std::false_type { };

template<typename T>
class IsTypeTag<TypeTag<T>> : public std::true_type { };

class MoveIfNoexcept { };

/** Contains the value of a variant. */
template<typename...>
union Union;

template<>
union Union<> { };

template<typename Head, typename... Tail>
union Union<Head, Tail...> {

    static_assert(
            std::is_same<Head, typename std::decay<Head>::type>::value,
            "Contained type must be decayed");

private:

    using TailUnion = Union<Tail...>;

    Head mHead;
    TailUnion mTail;

public:

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
    value() & noexcept {
        return mHead;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, const U &>::type
    value() const & noexcept {
        return mHead;
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<std::is_same<U, Head>::value, U &&>::type
    value() && noexcept {
        return std::move(mHead);
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, U &>::type
    value() & noexcept {
        return mTail.template value<U>();
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, const U &>::type
    value() const & noexcept {
        return mTail.template value<U>();
    }

    /** Returns a reference to the value of type <code>U</code>. */
    template<typename U>
    typename std::enable_if<!std::is_same<U, Head>::value, U &&>::type
    value() && noexcept {
        return std::move(mTail).template value<U>();
    }

    /**
     * Constructs the value of type <code>Head</code> using the given arguments.
     */
    template<typename... Arg>
    // constexpr XXX C++11 7.1.5.4
    explicit Union(TypeTag<Head>, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<Head, Arg...>::value) :
            mHead(std::forward<Arg>(arg)...) { }

    /**
     * Constructs the value of type <code>U</code> using the given arguments.
     */
    template<typename U, typename... Arg>
    // constexpr XXX C++11 7.1.5.4
    explicit Union(TypeTag<U> tag, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) :
            mTail(tag, std::forward<Arg>(arg)...) { }

    /**
     * Constructs the value by move-constructing the result of the argument
     * function, which must return (a reference to) an object of type Head.
     */
    template<typename F>
    // constexpr XXX C++11 7.1.5.4
    explicit Union(FunctionalInitialize, TypeTag<Head>, F &&f)
            noexcept(noexcept(std::declval<F>()()) &&
                    std::is_nothrow_constructible<
                        Head, typename std::result_of<F()>::type>::value) :
            mHead(std::forward<F>(f)()) { }

    /**
     * Constructs the value by move-constructing the result of the argument
     * function, which must return (a reference to) a value of one of the types
     * that may be contained in this union.
     */
    template<typename U, typename F>
    // constexpr XXX C++11 7.1.5.4
    explicit Union(FunctionalInitialize fi, TypeTag<U> tag, F &&f)
            noexcept(std::is_nothrow_constructible<
                TailUnion, FunctionalInitialize, TypeTag<U>, F &&>::value) :
            mTail(fi, tag, std::forward<F>(f)) { }

};

template<typename T, typename U>
using CopyReference = typename std::conditional<
        std::is_lvalue_reference<T>::value, U &, U &&>::type;

/** A visitor on the type tag which forwards to a visitor on the variant. */
template<typename Union, typename Visitor>
class Applier {

private:

    Union &&mTarget;
    Visitor &&mVisitor;

public:

    constexpr explicit Applier(Union &&target, Visitor &&visitor) noexcept :
            mTarget(std::forward<Union>(target)),
            mVisitor(std::forward<Visitor>(visitor)) { }

    template<typename T, typename R = CopyReference<Union, T>>
    constexpr auto operator()(TypeTag<T>) const
            noexcept(noexcept(std::declval<Visitor>()(std::declval<R>())))
            -> typename std::result_of<Visitor(R)>::type {
        return std::forward<Visitor>(mVisitor)(
                std::forward<Union>(mTarget).template value<T>());
    }

}; // template<typename Union> class Applier

/** A visitor that constructs a value of a target union. */
template<typename Union>
class Constructor {

private:

    Union &mTarget;

public:

    explicit Constructor(Union &target) noexcept : mTarget(target) { }

    template<
            typename T,
            typename V = typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>
    void operator()(T &&v)
            noexcept(std::is_nothrow_constructible<V, T &&>::value) {
        new (std::addressof(mTarget)) Union(TypeTag<V>(), std::forward<T>(v));
    }

};

/** A visitor that constructs a value of a target union. */
template<typename Union>
class MoveIfNoexceptConstructor {

private:

    Union &mTarget;

public:

    constexpr explicit MoveIfNoexceptConstructor(Union &target) noexcept :
            mTarget(target) { }

    template<typename T>
    void operator()(T &v)
            noexcept(std::is_nothrow_move_constructible<T>::value) {
        new (std::addressof(mTarget)) Union(
                TypeTag<T>(), std::move_if_noexcept(v));
    }

};

/** A visitor that assigns a value to a target union. */
template<typename Union>
class Assigner {

private:

    Union &mTarget;

public:

    explicit Assigner(Union &target) noexcept : mTarget(target) { }

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

    template<typename V>
    void operator()(V &v) noexcept(std::is_nothrow_destructible<V>::value) {
        v.~V();
    }

};

namespace {

/** A helper function that constructs an applier. */
template<typename Union, typename Visitor>
Applier<Union, Visitor> applier(Union &&u, Visitor &&v) noexcept {
    return Applier<Union, Visitor>(
            std::forward<Union>(u), std::forward<Visitor>(v));
}

/** A helper function that constructs a constructor. */
template<typename Union>
Constructor<Union> constructor(Union &target) noexcept {
    return Constructor<Union>(target);
}

template<typename Union>
MoveIfNoexceptConstructor<Union> moveIfNoexceptConstructor(Union &target)
        noexcept {
    return MoveIfNoexceptConstructor<Union>(target);
}

/** A helper function that constructs an assigner. */
template<typename Union>
Assigner<Union> assigner(Union &target) noexcept {
    return Assigner<Union>(target);
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
class IsNothrowSwappableImpl : public std::false_type { };

template<typename T>
class IsNothrowSwappableImpl<T, true> :
    public std::integral_constant<
            bool,
            noexcept(swap(std::declval<T &>(), std::declval<T &>()))> {
};

template<typename T>
using IsNothrowSwappable = IsNothrowSwappableImpl<T>;

template<typename Variant>
class Swapper {

private:

    Variant &mOther;

public:

    explicit Swapper(Variant &other) noexcept : mOther(other) { }

    template<typename T>
    void operator()(T &v) noexcept(IsNothrowSwappable<T>::value) {
        swap(v, mOther.template value<T>());
    }

};

} // namespace swap_impl

/** Fundamental implementation of variant. */
template<typename... T>
class VariantBase : private TypeTag<T...> {

    /*
     * The type tag sub-object is contained as a base class object rather than
     * a non-static data member to allow empty base optimization.
     */

public:

    /**
     * The type of the type tag which specifies the type of the contained
     * value.
     */
    using Tag = TypeTag<T...>;

private:

    using Value = Union<T...>;

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
            ForAll<swap_impl::IsNothrowSwappable, T...>::value &&
            IS_NOTHROW_MOVE_CONSTRUCTIBLE &&
            IS_NOTHROW_DESTRUCTIBLE;

    /** Returns the integral value that identifies the parameter type. */
    template<typename U>
    constexpr static Tag tag() noexcept {
        return TypeTag<U>();
    }

    /**
     * Returns an integral value that identifies the type of the currently
     * contained value.
     */
    Tag tag() const noexcept { return *this; }

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
    explicit VariantBase(TypeTag<U> tag, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<U, Arg...>::value) :
            Tag(tag), mValue(tag, std::forward<Arg>(arg)...) { }

    /**
     * Creates a new variant by copy- or move-constructing its contained value
     * from the argument.
     *
     * Throws any exception thrown by the constructor.
     *
     * @tparam U the argument type.
     * @tparam V the type of the new contained value to be constructed.
     *     Inferred from the argument type. If V is a specialization of
     *     TypeTag, this constructor overload cannot be used.
     * @param v the argument forwarded to the constructor.
     */
    template<
            typename U,
            typename V = typename std::decay<U>::type,
            typename = typename std::enable_if<IsAnyOf<V, T...>::value>::type,
            typename = typename std::enable_if<!IsTypeTag<V>::value>::type>
    VariantBase(U &&v)
            noexcept(std::is_nothrow_constructible<V, U &&>::value) :
            VariantBase(TypeTag<V>(), std::forward<U>(v)) { }

    /**
     * Creates a new variant by move-constructing its contained value from the
     * result of calling the argument function.
     *
     * Throws any exception thrown by the argument function or constructor.
     *
     * @tparam U the type of the new contained value to be constructed.
     * @tparam F the type of the function argument.
     * @param fi a dummy argument to disambiguate overload resolution.
     * @param tag a dummy object to select the type of the contained value.
     * @param f the function that constructs the new contained value.
     */
    template<typename U, typename F>
    VariantBase(FunctionalInitialize fi, TypeTag<U> tag, F &&f)
            noexcept(std::is_nothrow_constructible<
                    Value, FunctionalInitialize, TypeTag<U>, F &&>::value) :
            Tag(tag), mValue(fi, tag, std::forward<F>(f)) { }

    /**
     * Creates a new variant by move-constructing its contained value from the
     * result of calling the argument function.
     *
     * Throws any exception thrown by the argument function or constructor.
     *
     * @tparam F the type of the function argument.
     * @tparam U the type of the new contained value to be constructed.
     *     (inferred from the return type of the argument function.)
     * @param fi a dummy argument for overload resolution disambiguation.
     * @param f the function that constructs the new contained value.
     */
    template<
            typename F,
            typename U = typename std::decay<
                    typename std::result_of<F()>::type>::type>
    VariantBase(FunctionalInitialize fi, F &&f)
            noexcept(std::is_nothrow_constructible<
                VariantBase, FunctionalInitialize, TypeTag<U>, F &&>::value) :
            VariantBase(fi, TypeTag<U>(), std::forward<F>(f)) { }

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
        assert(tag() == tag<U>());
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
        assert(tag() == tag<U>());
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
     * @param visitor an object that has () operators to be called.
     * @return the return value of the () operator.
     */
    template<typename Visitor>
    auto apply(Visitor &&visitor) &
            noexcept(noexcept(std::declval<Tag &>().apply(
                    std::declval<Applier<Value &, Visitor>>())))
            -> typename CommonResult<Visitor, T &...>::type {
        return Tag::apply(applier(value(), std::forward<Visitor>(visitor)));
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
            noexcept(noexcept(std::declval<const Tag &>().apply(
                    std::declval<Applier<const Value &, Visitor>>())))
            -> typename CommonResult<Visitor, const T &...>::type {
        return Tag::apply(applier(value(), std::forward<Visitor>(visitor)));
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
            noexcept(noexcept(std::declval<Tag &>().apply(
                    std::declval<Applier<Value, Visitor>>())))
            -> typename CommonResult<Visitor, T &&...>::type {
        return Tag::apply(applier(
                    std::move(value()), std::forward<Visitor>(visitor)));
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
            Tag(v.tag()) {
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
            Tag(v.tag()) {
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
            Tag(v.tag()) {
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
            Tag(v.tag()) {
        std::move(v).apply(constructor(this->value()));
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
    VariantBase(MoveIfNoexcept, VariantBase<U...> &v)
            noexcept(VariantBase<U...>::IS_NOTHROW_MOVE_CONSTRUCTIBLE) :
            Tag(v.tag()) {
        v.apply(moveIfNoexceptConstructor(value()));
    }

private:

    /**
     * Re-constructs this variant by forwarding the argument to the
     * constructor. This function assumes the constructor never throws. If the
     * constructor did throw something at runtime, std::terminate is called.
     */
    template<typename... Arg>
    void reconstructOrTerminate(Arg &&... arg) noexcept {
        new (this) VariantBase(std::forward<Arg>(arg)...);
    }

public:

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * To ensure that a valid object is contained after return from this
     * function, nothing must be thrown by the destructor that destructs the
     * currently contained value or the constructor of the new value. If
     * something is thrown at runtime, std::terminate is called.
     *
     * @see #emplaceWithFallback
     * @see #emplaceWithBackup
     * @see #reset
     */
    template<typename... Arg>
    void emplace(Arg &&... arg) noexcept {
        this->~VariantBase();
        new (this) VariantBase(std::forward<Arg>(arg)...);
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * If the destructor or constructor threw something, the default
     * constructor of <code>Fallback</code> is called as a fallback and the
     * exception is re-thrown. If the <code>Fallback</code> default constructor
     * threw again, std::terminate is called.
     */
    template<typename Fallback, typename... Arg>
    void emplaceWithFallback(Arg &&... arg)
            noexcept(IS_NOTHROW_DESTRUCTIBLE &&
                std::is_nothrow_constructible<VariantBase, Arg &&...>::value) {
        try {
            this->~VariantBase();
            new (this) VariantBase(std::forward<Arg>(arg)...);
        } catch (...) {
            reconstructOrTerminate(TypeTag<Fallback>());
            throw;
        }
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * This overload is selected if the destructor and constructor are
     * guaranteed never to throw. This overload is equivalent to {@link
     * #emplace}.
     */
    template<typename... Arg>
    typename std::enable_if<
            IS_NOTHROW_DESTRUCTIBLE &&
            std::is_nothrow_constructible<VariantBase, Arg &&...>::value
    >::type
    emplaceWithBackup(Arg &&... arg) noexcept {
        return emplace(std::forward<Arg>(arg)...);
    }

    /**
     * Destructs the currently contained value and creates a new contained
     * value by calling the constructor of this variant again with the given
     * arguments.
     *
     * This overload is selected if the destructor or constructor may throw. In
     * case they throw something, a temporary backup of the original value is
     * created before the destruction and construction. If an exception is
     * thrown from the destructor or constructor, the backup is used to restore
     * the original value before propagating the exception. If the restoration
     * again fails with an exception, std::terminate is called. The backup is
     * created by std::move_if_noexcept.
     *
     * Requirements: All the contained types must be move-constructible.
     */
    template<typename... Arg>
    typename std::enable_if<
            !IS_NOTHROW_DESTRUCTIBLE ||
            !std::is_nothrow_constructible<VariantBase, Arg &&...>::value
    >::type
    emplaceWithBackup(Arg &&... arg) {
        VariantBase backup(MoveIfNoexcept(), *this);
        try {
            this->~VariantBase();
            new (this) VariantBase(std::forward<Arg>(arg)...);
        } catch (...) {
            reconstructOrTerminate(std::move(backup));
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
        emplace(TypeTag<V>(), std::forward<U>(v));
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
        if (tag() == tag<V>())
            value<V>() = std::forward<U>(v);
        else
            reset(std::forward<U>(v));
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
    AssignableVariant &operator=(const AssignableVariant &v)
            noexcept(Base::IS_NOTHROW_COPY_ASSIGNABLE) {
        if (this->tag() == v.tag())
            v.apply(assigner(this->value()));
        else
            this->emplaceWithBackup(v);
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
    AssignableVariant &operator=(AssignableVariant &&v)
            noexcept(Base::IS_NOTHROW_MOVE_ASSIGNABLE) {
        if (this->tag() == v.tag())
            std::move(v).apply(assigner(this->value()));
        else
            this->emplaceWithBackup(std::move(v));
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
            noexcept(Variant<U...>::IS_NOTHROW_COPY_ASSIGNABLE &&
                    Variant::IS_NOTHROW_MOVE_CONSTRUCTIBLE) {
        if (this->tag() == typename Base::Tag(v.tag()))
            v.apply(assigner(this->value()));
        else
            this->emplaceWithBackup(static_cast<const VariantBase<U...> &>(v));
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
            noexcept(Variant<U...>::IS_NOTHROW_MOVE_ASSIGNABLE &&
                    Variant::IS_NOTHROW_MOVE_CONSTRUCTIBLE) {
        if (this->tag() == typename Base::Tag(v.tag()))
            std::move(v).apply(assigner(this->value()));
        else
            this->emplaceWithBackup(static_cast<VariantBase<U...> &&>(v));
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
     * move-constructible, and no-throw destructible.
     */
    void swap(Variant &other) noexcept(Variant::IS_NOTHROW_SWAPPABLE) {
        if (this->tag() == other.tag())
            return this->apply(swap_impl::Swapper<Variant<T...>>(other));

        assert(this != &other);

        Variant<T...> temporary(std::move(*this));
        this->emplace(std::move(other));
        other.emplace(std::move(temporary));
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
 * Requirements: All the contained types must be swappable, no-throw
 * move-constructible, and no-throw destructible.
 */
template<typename... T>
void swap(Variant<T...> &a, Variant<T...> &b)
        noexcept(Variant<T...>::IS_NOTHROW_SWAPPABLE) {
    a.swap(b);
}

} // namespace variant_impl

using variant_impl::Variant;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_Variant_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
