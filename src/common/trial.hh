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

#ifndef INCLUDED_common_trial_hh
#define INCLUDED_common_trial_hh

#include "buildconfig.h"

#include <exception>
#include <memory>
#include "common/variant.hh"
#include "helpermacros.h"

namespace sesh {
namespace common {

/**
 * Result of a computation that may fail with an exception. It is either a
 * value of the template parameter type or an exception.
 */
template<typename T>
class trial : public common::variant<T, std::exception_ptr> {

public:

    using common::variant<T, std::exception_ptr>::variant;

    /** Checks if this value has an actual result rather than an exception. */
    explicit operator bool() const noexcept {
        switch (this->tag()) {
        case trial::template tag<T>():
            return true;
        case trial::template tag<std::exception_ptr>():
            return false;
        }
        UNREACHABLE();
    }

    /** Returns a reference to the result value or throws the exception. */
    T &get() {
        switch (this->tag()) {
        case trial::template tag<T>():
            return this->template value<T>();
        case trial::template tag<std::exception_ptr>():
            std::rethrow_exception(this->template value<std::exception_ptr>());
        }
        UNREACHABLE();
    }

    /** Returns a reference to the result value or throws the exception. */
    const T &get() const {
        switch (this->tag()) {
        case trial::template tag<T>():
            return this->template value<T>();
        case trial::template tag<std::exception_ptr>():
            std::rethrow_exception(this->template value<std::exception_ptr>());
        }
        UNREACHABLE();
    }

    /**
     * Returns a reference to the result value. This function can be called
     * only when this trial has a valid result value.
     */
    T &operator*() {
        return this->template value<T>();
    }
    /**
     * Returns a reference to the result value. This function can be called
     * only when this trial has a valid result value.
     */
    const T &operator*() const {
        return this->template value<T>();
    }

    /** Returns a pointer to the result value or throws the exception. */
    T *operator->() {
        return std::addressof(get());
    }

    /** Returns a pointer to the result value or throws the exception. */
    const T *operator->() const {
        return std::addressof(get());
    }

}; // template<typename T> class trial

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_trial_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
