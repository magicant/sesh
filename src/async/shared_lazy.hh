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

#ifndef INCLUDED_async_shared_lazy_hh
#define INCLUDED_async_shared_lazy_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "async/lazy.hh"

namespace sesh {
namespace async {

/**
 * A shared lazy is contains a {@link lazy} that is shared with other shared
 * lazy objects. It is basically a shared pointer to a lazy object, but the
 * pointer can never be null.
 *
 * @tparam T Type of lazily computed value (optionally const-qualified).
 */
template<typename T>
class shared_lazy {

public:

    /** Type of the lazy object that is shared. */
    using lazy_type = typename std::conditional<
            std::is_const<T>::value,
            const class lazy<typename std::remove_const<T>::type>,
            class lazy<T>>::type;

    using result_type = typename lazy_type::result_type;
    using value_type = typename std::conditional<
            std::is_const<T>::value,
            const common::trial<result_type>,
            common::trial<result_type>>::type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    using result_maker = typename lazy_type::result_maker;

private:

    std::shared_ptr<lazy_type> m_shared_ptr;

public:

    /**
     * Constructs a shared lazy that contains a new default-constructed lazy.
     */
    shared_lazy() : m_shared_ptr(std::make_shared<lazy_type>()) { }

    /** Constructs a shared lazy value with an already computed value. */
    shared_lazy(const result_type &v) :
            m_shared_ptr(std::make_shared<lazy_type>(v)) { }

    /** Constructs a shared lazy value with an already computed value. */
    shared_lazy(result_type &&v) :
            m_shared_ptr(std::make_shared<lazy_type>(std::move(v))) { }

    /**
     * Constructs a lazy value with a value computing function. The function
     * will be called once when the value is needed.
     */
    explicit shared_lazy(const result_maker &f) :
            m_shared_ptr(std::make_shared<lazy_type>(f)) { }

    /**
     * Constructs a lazy value with a value computing function. The function
     * will be called once when the value is needed.
     */
    explicit shared_lazy(result_maker &&f) :
            m_shared_ptr(std::make_shared<lazy_type>(std::move(f))) { }

    /**
     * Constructs a lazy value with an existing lazy object to be shared.
     * @param p Shared pointer managing the shared lazy object @c l.
     * @param l Lazy object to be shared.
     */
    template<typename U>
    shared_lazy(const std::shared_ptr<U> &p, lazy_type &l) noexcept :
            m_shared_ptr(p, std::addressof(l)) { }

    shared_lazy(const shared_lazy &) = default;
    shared_lazy &operator=(const shared_lazy &) = default;
    // Move constructor and assignment operator are not declared so that copy
    // constructor and assignment operator are always used to ensure the shared
    // lazy always has a non-null pointer.

    const std::shared_ptr<lazy_type> &shared_ptr() const noexcept {
        return m_shared_ptr;
    }

    lazy_type &lazy() const noexcept {
        return *shared_ptr();
    }

    /** Returns true iff the value has been computed. */
    bool has_computed() const noexcept {
        return lazy().has_computed();
    }

    /** Returns true iff the value is currently being computed. */
    bool is_computing() const noexcept {
        return lazy().is_computing();
    }

    reference operator*() const noexcept {
        return lazy().operator*();
    }

    pointer operator->() const noexcept {
        return lazy().operator->();
    }

}; // template<typename T> class shared_lazy

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_shared_lazy_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
