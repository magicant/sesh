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

#ifndef INCLUDED_async_lazy_hh
#define INCLUDED_async_lazy_hh

#include "buildconfig.h"

#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "common/direct_initialize.hh"
#include "common/either.hh"
#include "common/functional_initialize.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace sesh {
namespace async {

/**
 * This type of exception is thrown when a lazily computed value is accessed
 * while it is being computed.
 */
class lazy_error : public std::logic_error {
public:
    lazy_error() : std::logic_error("The value is being computed.") { }
};

/**
 * The lazy class template encapsulates a lazily computed value.
 *
 * An instance of lazy may be constructed without having a computed value. When
 * the value is accessed, the value is constructed then and a reference to it
 * is returned. Once the lazy object returns the same reference to the value;
 * the value is never constructed twice for a single lazy instance.
 *
 * If an instance of lazy is copy- or move-constructed from another lazy
 * instance whose value is just being computed, the new lazy instance will
 * never have a value. Its value will be considered as being computed forever.
 *
 * @tparam T Type of the value that is lazily computed. It must be a decayed
 * type other than std::exception_ptr.
 */
template<typename T>
class lazy {

public:

    using result_type = T;
    using value_type = common::trial<result_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    using result_maker = std::function<result_type()>;

private:

    static result_type default_initialize() { return result_type(); }

    /** Tag type to denote the state where the value is being computed. */
    class computing { };

    using thunk_type = common::variant<value_type, computing, result_maker>;

    mutable thunk_type m_thunk;

    struct result_making_visitor : std::reference_wrapper<const lazy> {

        using std::reference_wrapper<const lazy>::reference_wrapper;

        reference operator()(value_type &v) {
            return v;
        }

        reference operator()(computing) {
            throw lazy_error();
        }

        reference operator()(result_maker &f) {
            result_maker f_copy = std::move(f);
            thunk_type &thunk = this->get().m_thunk;
            thunk.template emplace(computing());

            try {
                thunk.template emplace_with_fallback<computing>(
                        common::direct_initialize(),
                        common::type_tag<value_type>(),
                        f_copy());
            } catch (...) {
                thunk.template emplace(
                        common::direct_initialize(),
                        common::type_tag<value_type>(),
                        std::current_exception());
            }
            return operator()(thunk.template value<value_type>());
        }

    };

    reference compute_value() const {
        return m_thunk.apply(result_making_visitor(*this));
    }

public:

    /**
     * Constructs a lazy value. When the value is needed, it is constructed
     * using its default constructor (value-initialization).
     */
    lazy() noexcept :
            m_thunk(thunk_type::template create<result_maker>(
                        &lazy::default_initialize)) { }

    /** Constructs a lazy value with an already computed value. */
    lazy(const result_type &v)
            noexcept(std::is_nothrow_copy_constructible<result_type>::value) :
            m_thunk(
                    common::direct_initialize(),
                    common::type_tag<value_type>(),
                    v) { }

    /** Constructs a lazy value with an already computed value. */
    lazy(result_type &&v)
            noexcept(std::is_nothrow_move_constructible<result_type>::value) :
            m_thunk(
                    common::direct_initialize(),
                    common::type_tag<value_type>(),
                    std::move(v)) { }

    /**
     * Constructs a lazy value with a value computing function. The function
     * will be called once when the value is needed.
     */
    explicit lazy(const result_maker &f) : m_thunk(f) { }

    /**
     * Constructs a lazy value with a value computing function. The function
     * will be called once when the value is needed.
     */
    explicit lazy(result_maker &&f) : m_thunk(std::move(f)) { }

    /** Returns true iff the value has been computed. */
    bool has_computed() const noexcept {
        return m_thunk.tag() == common::type_tag<value_type>();
    }

    /** Returns true iff the value is currently being computed. */
    bool is_computing() const noexcept {
        return m_thunk.tag() == common::type_tag<computing>();
    }

    /**
     * Returns a reference to the lazily computed value.
     *
     * If the value is not yet computed, it is computed before the reference is
     * returned.
     *
     * @throws lazy_error If this operator is used while the value is being
     * computed.
     */
    reference operator*() {
        return compute_value();
    }

    /**
     * Returns a reference to the lazily computed value.
     *
     * If the value is not yet computed, it is computed before the reference is
     * returned.
     *
     * @throws lazy_error If this operator is used while the value is being
     * computed.
     */
    const_reference operator*() const {
        return compute_value();
    }

    /**
     * Returns a pointer to the lazily computed value.
     *
     * If the value is not yet computed, it is computed before the pointer is
     * returned.
     *
     * @throws lazy_error If this operator is used while the value is being
     * computed.
     */
    pointer operator->() {
        return std::addressof(compute_value());
    }

    /**
     * Returns a pointer to the lazily computed value.
     *
     * If the value is not yet computed, it is computed before the pointer is
     * returned.
     *
     * @throws lazy_error If this operator is used while the value is being
     * computed.
     */
    const_pointer operator->() const {
        return std::addressof(compute_value());
    }

}; // template<typename T> class lazy

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_lazy_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
