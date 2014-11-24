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

#ifndef INCLUDED_common_either_hh
#define INCLUDED_common_either_hh

#include "buildconfig.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "common/direct_initialize.hh"
#include "common/empty.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace sesh {
namespace common {

/**
 * This exception is thrown when the value of failed maybe value is accessed.
 */
class bad_maybe_access : public std::logic_error {
public:
    using std::logic_error::logic_error;
}; // class bad_maybe_access

/** Throws bad_maybe_access. */
[[noreturn]] inline void rethrow_failed_either(empty) {
    throw bad_maybe_access("empty maybe");
}

/**
 * Re-throws the exception pointed to by the given exception pointer. If the
 * pointer is null, std::invalid_argument is thrown.
 */
[[noreturn]] inline void rethrow_failed_either(std::exception_ptr p) {
    if (p)
        std::rethrow_exception(p);
    throw std::logic_error("null exception pointer");
}

/**
 * A specialization of this class template is used to create a fallback value
 * from a failed result of {@link either}.
 *
 * The default specialization of this template is no-throw default
 * constructible.
 *
 * This template can be freely specialized to override the default behavior.
 * Any specialization have the call operator that always throws.
 *
 * @tparam L The error type of the either for which is fallback is used.
 * @tparam R The value type of the either for which is fallback is used.
 */
template<typename L, typename R>
class either_fallback {

public:

    /** Calls rethrow_failed_either with the argument. */
    [[noreturn]] void operator()(const L &error) const {
        rethrow_failed_either(error);
    }

}; // template<typename L, typename R> class either_fallback

/**
 * Either is a twofold variant to represent either successful or unsuccessful
 * results. Like Haskell's Either type, the definition of this class template
 * is <em>right-biased</em>; the left value stands for an unsuccessful result
 * and right for successful. If the two types should be treated equally, you
 * should use {@link variant_impl::variant} directly.
 *
 * @tparam L The type of failed results.
 * @tparam R The type of successful results.
 */
template<typename L, typename R>
class either : public variant<L, R> {

public:

    using error_type = L;
    using value_type = R;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    using variant<L, R>::variant;

    /** Default-constructs an error_type value. */
    constexpr either()
            noexcept(
                    std::is_nothrow_default_constructible<error_type>::value) :
            variant<L, R>(direct_initialize(), type_tag<error_type>()) { }

    /**
     * Tests if this is a successful result.
     * @returns true if <code>this->tag() == type_tag<value_type>()</code>,
     * false if <code>this->tag() == type_tag<error_type>()</code>.
     */
    constexpr explicit operator bool() const noexcept {
        return !(this->tag() == type_tag<error_type>());
    }

    /**
     * Returns a reference to the successful result value. This operator can be
     * used only when the contained value is of {@link #value_type}; otherwise,
     * the behavior is undefined.
     */
    /* constexpr */ reference operator*() {
        return this->template value<value_type>();
    }
    /**
     * Returns a reference to the successful result value. This operator can be
     * used only when the contained value is of {@link #value_type}; otherwise,
     * the behavior is undefined.
     */
    constexpr const_reference operator*() const {
        return this->template value<value_type>();
    }

    /**
     * Returns a pointer to the successful result value. This operator can be
     * used only when the contained value is of {@link #value_type}; otherwise,
     * the behavior is undefined.
     */
    /* constexpr */ pointer operator->() {
        return std::addressof(**this);
    }
    /**
     * Returns a pointer to the successful result value. This operator can be
     * used only when the contained value is of {@link #value_type}; otherwise,
     * the behavior is undefined.
     */
    constexpr const_pointer operator->() const {
        return std::addressof(**this);
    }

    /**
     * Returns a reference to the successful result, if any.
     *
     * If this either value has a failed result, it is passed to the argument
     * to throw an exception.
     */
    /* constexpr */ reference get(
            either_fallback<L, R> f = either_fallback<L, R>()) {
        return *this || (f(this->template value<error_type>()), 0), **this;
    }
    /**
     * Returns a reference to the successful result, if any.
     *
     * If this either value has a failed result, it is passed to the argument
     * to throw an exception.
     */
    constexpr const_reference get(
            either_fallback<L, R> f = either_fallback<L, R>()) const {
        return *this || (f(this->template value<error_type>()), 0), **this;
    }

    /**
     * Returns a reference to the successful result value if any, or the
     * argument otherwise.
     */
    /* constexpr */ reference value_or(reference alternate) & noexcept {
        return *this ? **this : alternate;
    }
    /**
     * Returns a reference to the successful result value if any, or the
     * argument otherwise.
     */
    constexpr const_reference value_or(const_reference alternate) const &
            noexcept {
        return *this ? **this : alternate;
    }
    /**
     * Returns a reference to the successful result value if any, or the
     * argument otherwise.
     */
    /* constexpr */ value_type &&value_or(value_type &&alternate) && noexcept {
        return std::move(*this ? **this : alternate);
    }

    /**
     * Destructs the current value and constructs a new successful result.
     *
     * If the constructor throws, this either object is reinitialized by
     * default construction of error_type before the exception is re-thrown. If
     * the default constructor of error_type throws again, the program is
     * <code>std::terminate</code>d.
     */
    template<typename... Arg>
    /* constexpr */ void try_emplace(Arg &&... arg) {
        this->template emplace_with_fallback<error_type>(
                direct_initialize(),
                type_tag<value_type>(),
                std::forward<Arg>(arg)...);
    }
    // XXX support initializer_list?

    /**
     * Destructs the current value and resets this object to the
     * default-constructed error_type value. If the default constructor of
     * error_type throws, the program is <code>std::terminate</code>d.
     */
    /* constexpr */ void clear() noexcept {
        this->emplace(direct_initialize(), type_tag<error_type>());
    }

    /**
     * Assigns the argument to this either object as a successful result. If
     * the current value is a failed result, a new contained object is
     * copy/move constructed from the argument. Otherwise, the argument is
     * assigned to the existing result.
     *
     * If the constructor throws, this either object is reinitialized by
     * default construction of error_type before the exception is re-thrown. If
     * the default constructor of error_type throws again, the program is
     * <code>std::terminate</code>d.
     *
     * @tparam T the type of the assigned object.
     * @param v the assigned object.
     * @see #try_emplace
     */
    template<typename T>
    /* constexpr */ auto operator=(T &&v)
            noexcept(std::is_nothrow_constructible<value_type, T &&>::value &&
                    std::is_nothrow_assignable<value_type &, T &&>::value)
            -> typename std::enable_if<
                    std::is_constructible<value_type, T &&>::value &&
                            std::is_assignable<value_type &, T &&>::value,
                    either &>::type {
        if (*this)
            **this = std::forward<T>(v);
        else
            try_emplace(std::forward<T>(v));
        return *this;
    }

}; // template<typename L, typename R> class either

template<typename T>
using maybe = either<empty, T>;

/**
 * Creates a new maybe object that contains the argument. The contained object
 * is directly constructed by calling its constructor.
 *
 * Propagates any exception thrown by the constructor.
 *
 * @tparam T the type of the new contained object.
 * @tparam Arg constructor argument types
 * @param arg arguments to the contained object's constructor.
 */
template<typename T, typename... Arg>
maybe<T> make_maybe(Arg &&... arg)
        noexcept(std::is_nothrow_constructible<T, Arg &&...>::value) {
    return maybe<T>(
            direct_initialize(), type_tag<T>(), std::forward<Arg>(arg)...);
}

/**
 * Creates a new maybe object that contains the argument. This is a
 * single-argument version of {@link make_maybe} that allows inference of the
 * contained type.
 *
 * Propagates any exception thrown by the constructor.
 *
 * @tparam T the (usually inferred) type of the argument.
 * @tparam U the actual type of the new contained object.
 * @param v a reference to the original value.
 */
template<typename T, typename U = typename std::decay<T>::type>
maybe<U> make_maybe_of(T &&v)
        noexcept(std::is_nothrow_constructible<
                maybe<U>, direct_initialize, type_tag<U>, T &&>::value) {
    return maybe<U>(direct_initialize(), type_tag<U>(), std::forward<T>(v));
}

template<typename T>
using trial = either<std::exception_ptr, T>;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_either_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
