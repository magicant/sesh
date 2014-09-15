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

#ifndef INCLUDED_common_maybe_hh
#define INCLUDED_common_maybe_hh

#include "buildconfig.h"

#include <memory>
#include <type_traits>
#include <utility>
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace sesh {
namespace common {

/**
 * <code>maybe&lt;T></code> is a container that contains zero or one instance
 * of <code>T</code>. Maybe allows construction and destruction of the
 * contained object at any time.
 *
 * @tparam T the type of the possibly contained object. It must be a
 * non-pointer decayed type (see std::decay).
 */
template<typename T>
class maybe {

private:

    /*
     * Pointers cannot be contained in maybe objects because pointers are
     * themselves nullable.
     */
    static_assert(!std::is_pointer<T>::value,
            "Maybe cannot contain pointer");
    static_assert(!std::is_member_pointer<T>::value,
            "Maybe cannot contain member pointer");

    class nil { };

    variant<nil, T> m_value;

public:

    /** Constructs a maybe object that contains nothing. */
    maybe() noexcept : m_value(type_tag<nil>()) { }

    /**
     * Constructs a non-empty maybe object by directly constructing the
     * contained object. The arguments (except the tag) are passed to the
     * applicable constructor of the contained object.
     *
     * @tparam Arg constructor argument types
     * @param tag a dummy object to disambiguate overload resolution.
     * @param arg arguments to the contained object's constructor.
     */
    template<typename... Arg>
    explicit maybe(type_tag<T> tag, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<T, Arg...>::value) :
            m_value(tag, std::forward<Arg>(arg)...) { }
    // XXX support initializer_list?

    /**
     * Constructs a non-empty maybe object by copy-constructing the contained
     * object.
     */
    maybe(const T &v) noexcept(std::is_nothrow_copy_constructible<T>::value) :
            m_value(type_tag<T>(), v) { }

    /**
     * Constructs a non-empty maybe object by move-constructing the contained
     * object.
     */
    maybe(T &&v) noexcept(std::is_nothrow_move_constructible<T>::value) :
            m_value(type_tag<T>(), std::move(v)) { }

    maybe(const maybe &) = default;
    maybe(maybe &&) = default;
    maybe &operator=(const maybe &) = default;
    maybe &operator=(maybe &&) = default;
    // XXX: GCC 4.8.1 rejects implicit exception-specification in an explicitly
    // defaulted destructor declaration if the destructor allows some
    // exception.
    /* ~maybe() = default; */

    /**
     * Destructs the currently contained object (if any) and constructs a new
     * one.
     *
     * If the destructor or constructor throws, the maybe object is left empty.
     *
     * @tparam Arg constructor argument types
     * @param arg arguments to the contained object's constructor.
     */
    template<typename... Arg>
    void try_emplace(Arg &&... arg)
            noexcept(std::is_nothrow_destructible<T>::value &&
                    std::is_nothrow_constructible<T, Arg...>::value) {
        m_value.template emplace_with_fallback<nil>(
                type_tag<T>(), std::forward<Arg>(arg)...);
    }
    // XXX support initializer_list?

    /**
     * Makes this maybe object empty, destructing the currently contained
     * object if any.
     */
    void clear() noexcept(std::is_nothrow_destructible<T>::value) {
        m_value.template emplace_with_fallback<nil>(type_tag<nil>());
    }

    /** Returns true if and only if this maybe object is non-empty. */
    explicit operator bool() const noexcept {
        return m_value.tag() != m_value.template tag<nil>();
    }

    /**
     * Returns a reference to the contained object. The maybe object must not
     * be empty.
     */
    T &value() noexcept { return m_value.template value<T>(); }

    /**
     * Returns a reference to the contained object. The maybe object must not
     * be empty.
     */
    const T &value() const noexcept { return m_value.template value<T>(); }

    /**
     * Returns a reference to the contained object. The maybe object must not
     * be empty.
     */
    T &operator*() noexcept { return value(); }

    /**
     * Returns a reference to the contained object. The maybe object must not
     * be empty.
     */
    const T &operator*() const noexcept { return value(); }

    /**
     * Returns a pointer to the contained object. The maybe object must not be
     * empty.
     */
    T *operator->() noexcept { return std::addressof(value()); }

    /**
     * Returns a pointer to the contained object. The maybe object must not be
     * empty.
     */
    const T *operator->() const noexcept { return std::addressof(value()); }

    /**
     * Returns {@link #value()} if non-empty. Otherwise, returns a reference to
     * the argument.
     * @param alternative an object to which reference is returned if this
     * maybe object is empty.
     */
    T &value_or(T &alternative) noexcept {
        return *this ? value() : alternative;
    }

    /**
     * Returns {@link #value()} if non-empty. Otherwise, returns a reference to
     * the argument.
     * @param alternative an object to which reference is returned if this
     * maybe object is empty.
     */
    const T &value_or(const T &alternative) const noexcept {
        return *this ? value() : alternative;
    }

    /**
     * Assigns the argument to this maybe object. If the maybe is empty, a new
     * contained object is copy/move constructed from the argument. Otherwise,
     * the argument is assigned to the existing contained object.
     *
     * If the constructor throws, the maybe is left empty.
     *
     * @tparam U the type of the assigned object.
     * @param v the assigned object.
     */
    template<
            typename U,
            typename = typename std::enable_if<
                    std::is_constructible<T, U &&>::value &&
                    std::is_assignable<T &, U &&>::value>::type>
    maybe &operator=(U &&v)
            noexcept(std::is_nothrow_constructible<T, U &&>::value &&
                    std::is_nothrow_assignable<T &, U &&>::value) {
        if (*this)
            value() = std::forward<U>(v);
        else
            try_emplace(std::forward<U>(v));
        return *this;
    }

    /**
     * Swaps the contained objects of this and the argument maybe object.
     *
     * If the two are both non-empty, their contained values are swapped by the
     * swap function. If one is non-empty and the other empty, the contained
     * object is moved by the move (or copy) constructor and the original is
     * destructed. If both empty, nothing happens.
     *
     * Requirements: The contained type must be swappable and
     * move-constructible.
     */
    void swap(maybe &other)
            noexcept(decltype(m_value)::is_nothrow_swappable &&
                    decltype(m_value)::is_nothrow_move_constructible) {
        if (*this) {
            if (other) {
                using std::swap;
                swap(this->value(), other.value());
            } else {
                other.try_emplace(std::move(this->value()));
                this->clear();
            }
        } else {
            if (other) {
                this->try_emplace(std::move(other.value()));
                other.clear();
            } else {
                // Nothing to do.
            }
        }
    }

};

/**
 * Swaps two maybe objects.
 *
 * If the two are both non-empty, their contained values are swapped by the
 * swap function. If one is non-empty and the other empty, the contained object
 * is moved by the move (or copy) constructor and the original is destructed.
 * If both empty, nothing happens.
 *
 * Requirements: The contained type must be swappable and move-constructible.
 */
template<typename T>
void swap(maybe<T> &a, maybe<T> &b) noexcept(noexcept(a.swap(b))) {
    a.swap(b);
}

/**
 * Compares two maybe objects.
 *
 * Two empty maybe objects are equal. Empty and non-empty objects are unequal.
 * Two non-empty objects are equal if and only if their contained objects are
 * equal.
 *
 * Requirements: The contained objects must be comparable with the == operator.
 */
template<typename T>
bool operator==(const maybe<T> &a, const maybe<T> &b)
        noexcept(noexcept(a.value() == b.value())) {
    if (a)
        return b && a.value() == b.value();
    else
        return !b;
}

/**
 * Compares two maybe objects.
 *
 * If the arguments are both non-empty, their contained objects are compared.
 * Otherwise, this operator returns true if and only if the left-hand-side is
 * empty and the right-hand-side is non-empty.
 *
 * Requirements: The contained objects must be comparable with the &lt;
 * operator.
 */
template<typename T>
bool operator<(const maybe<T> &a, const maybe<T> &b)
        noexcept(noexcept(a.value() < b.value())) {
    return b && (!a || a.value() < b.value());
}

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
    return maybe<T>(type_tag<T>(), std::forward<Arg>(arg)...);
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
        noexcept(std::is_nothrow_constructible<maybe<U>, type_tag<U>, T &&>::
                value) {
    return maybe<U>(type_tag<U>(), std::forward<T>(v));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_maybe_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
